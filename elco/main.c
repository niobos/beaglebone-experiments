#include <pru/constant_table.h>
#include <pru/cfg.h>
#include <pru/intc.h>
#include <am335x/gpio.h>
#include <linux/pru.h>
#include <linux/virtio_config.h>
#include <pru/io.h>
#include <pru/ctrl.h>

#include <string.h> // memcpy()

#include "lib/rpmsg/rpmsg.h"
#include "lib/time/time.h"

#include "resource_table.h"

#include "abi.h"

#define CHAN_NAME "rpmsg-pru"   // needs to match driver on Linux side

#define MAX_MSG_SIZE 32
#define BIT_RATE 1 //4800
#define START_BIT_VALUE 1
#define PARITY_ENABLED 1
#define PARITY_MODE 0 // 0=even, 1=odd
#define STOP_BITS 1 // integer only, 1.5 not supported
#define STOP_BIT_VALUE 0

struct rpmsg_transport transport;
unsigned int rpmsg_src, rpmsg_dst;

uint8_t payload[RPMSG_BUF_SIZE];

struct owuart_state { // one-wire uart
	enum {
		OW_Idle = 0,
		OW_TxStart,
		OW_TxBit,
		OW_TxWaitForHalfBit,
		OW_TxMonitorBit,
		OW_TxWaitForFullBit,
		OW_TxArbitrated,
		OW_TxCompleted,
		OW_RxStartBit,
		OW_RxWaitForNextBit,
		OW_RxBit,
		OW_RxByte,
		OW_StartHoldOff,
		OW_HoldOff,
		OW_EndOfMessage
	} state;

	unsigned int c_tx, c_rx, c_framing_error, c_arbitrated; // counters

	unsigned int rx_bit, tx_bit;
	unsigned int tx_buffer_len;
	uint8_t tx_buffer[MAX_MSG_SIZE];
	uint8_t rx_buffer[MAX_MSG_SIZE];
} owuart;

// TODO: unmock
unsigned int line = 1;
inline unsigned int read() {
	return line;
}
inline void write(unsigned int b) {
	line = b;
	if( b ) GPIO1.SETDATAOUT = 1<<21;
	else GPIO1.CLEARDATAOUT = 1<<21;
}

void process_tick() {
	switch(owuart.state) {
	case OW_Idle: {
		if( read() == 1 ) {
			owuart.state = OW_RxStartBit;
			return;
		}

		if( owuart.tx_buffer_len ) {
			owuart.state = OW_TxStart;
			return;
		}

		unsigned int src, dst, len;
		if(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
			rpmsg_src = src;
			rpmsg_dst = dst;
			memcpy(owuart.tx_buffer, payload, len);
			owuart.tx_buffer_len = len;
			// remain in idle state, will catch this on next tick;
		}

		return;
	}

	case OW_TxStart:
		timer_arm(owuart.timer, CPU_FREQ/BIT_RATE/2, CPU_FREQ/BIT_RATE/2);
		owuart.tx_bit = 0x000000fe;
		owuart.state = OW_TxFindNextBit;
		break;

	case OW_TxFindNextBit: {
		unsigned int byte;
		asm(/* Use tx_bit as bit-struct:
			 * bits 15-8: byte to sent
			 * bits 7-0 : bit to sent
			 *            0xff: start bit
			 *            0-7: data bit
			 *            8- stop bit(s)
			 * bit 24: bit to Tx (to compare with Rx)
			 */
			"ADD %[tx_bit].b0, %[tx_bit].b0, 1\n\t"

			"QBNE within_byte, %[tx_bit].b0, %[nbits]\n\t"
			// next byte
			"ADD %[tx_bit].b1, %[tx_bit].b1, 1\n\t"
			"LDI %[tx_bit].b0, 0xff\n\t"

			"within_byte:\n\t"
			"MOV %[byte], %[tx_bit].b1\n\t"
		 	: /* outputs */ [tx_bit] "+r" (owuart.tx_bit), [byte] "=r" (byte)
			: /* inputs */ [nbits] "i" (8+PARITY_ENABLED+STOP_BITS+1) // databits + parity + stop bits + 1
			: /* clobber */
		);
		if( byte >= owuart.tx_buffer_len) owuart.state = OW_TxCompleted;
		else owuart.state = OW_TxBit;
		return;
	}

	case OW_TxBit: {
		unsigned int bit;
		asm(
			"QBNE not_start_bit, %[tx_bit].b0, 0xff\n\t"
			"LDI %[bit], %[start_bit_value]\n\t" // Start Bit
			"QBA end\n\t"

			"not_start_bit:\n\t"
			"LBBO %[bit], %[buf], %[tx_bit].b1, 1\n\t"
			"QBGT not_data_bit, %[tx_bit].b0, 8\n\t" // bit >= 8
			"LSR %[bit], %[bit], %[tx_bit].b0\n\t"
			"AND %[bit], %[bit], 0x01\n\t"
			"QBA end\n\t"

			"not_data_bit:\n\t"
#if PARITY_ENABLED == 1
			"QBNE not_parity_bit, %[tx_bit].b0, 8\n\t"
			"LOOP end_of_loop, 7\n\t"
			"XOR %[bit].t1, %[bit].t1, %[bit].t0\n\t" // XOR bit 0 in to bit 1
			"LSR %[bit], %[bit], 1\n\t" // bit >>= 1
			"end_of_loop:\n\t"
			"XOR %[bit].t0, %[bit].t0, %[parity_mode]"
			"QBA end\n\t"
#endif
			"not_parity_bit:\n\t"
			"LDI %[bit], %[stop_bit_value]\n\t" // Stop bit

			"end:\n\t"
			"MOV %[tx_bit].t24, %[bit]\n\t"
			: /* outputs */ [tx_bit] "+r" (owuart.tx_bit),
			                [bit] "=r" (bit)
			: /* inputs */  [buf] "r" (owuart.tx_buffer),
			                [start_bit_value] "i" (START_BIT_VALUE),
			                [parity_mode] "i" (PARITY_MODE),
							[stop_bit_value] "i" (STOP_BIT_VALUE)
			: /* clobber */
		);
		write(bit);
		owuart.state = OW_TxWaitForHalfBit;
		return;
	}

	case OW_TxWaitForHalfBit:
		if( timer_expired(owuart.timer) ) owuart.state = OW_TxMonitorBit;
		// else stay in this state
		break;

	case OW_TxMonitorBit: {
		unsigned int bit;
		asm("MOV %[tx_bit].t24, %[bit]\n\t"
			: /* outputs */ [bit] "=r" (bit)
			: /* inputs */  [tx_bit] "r" (owuart.tx_bit)
			: /* clobber */
		);
		if( read() == bit ) {
			owuart.state = OW_TxWaitForFullBit;
		} else {
			owuart.state = OW_TxArbitrated;
		}
		return;
	}

	case OW_TxWaitForFullBit:
		if( timer_expired(owuart.timer) ) owuart.state = OW_TxFindNextBit;
		break;

	case OW_TxArbitrated:
		// TODO
		break;

	case OW_TxCompleted:
		rpmsg_send(&transport, rpmsg_dst, rpmsg_src,
		           owuart.tx_buffer, owuart.tx_buffer_len)
		          );
		owuart.tx_buffer_len = 0;
		owuart.state = OW_StartHoldOff;
		break;

	case OW_RxStartBit:
		timer_arm(owuart.timer, CPU_FREQ/BIT_RATE/2, CPU_FREQ/BIT_RATE);
		owuart.state = OW_RxWaitForNextBit;
		break;

	case OW_RxWaitForNextBit:
		if( timer_expired(owuart.timer) ) owuart.state = OW_RxBit;
		break;

	case OW_RxBit: {
		unsigned int bit;
		asm(/* rx_bit is a bit-struct:
			 * bits 31-16: received bits (aligned left)
			 * bits 15-8: byte
			 * bits 7-0: bit
			 */
			"LSR %[rx_bit].w3, %[rx_bit].w3, 1\n\t"
			"MOV %[rx_bit].t31, %[in]\n\t"

			"ADD %[rx_bit].b0, %[rx_bit].b0, 1\n\t"
			"MOV %[bit], %[rx_bit].b0\n\t"
			: /* outputs */ [rx_bit] "+r" (owuart.rx_bit), [bit] "r" (bit)
			: /* inputs */ [in] "r" (read())
			: /* clobber */
		);
		if( bit >= 1+8+PARITY_ENABLED+1 ) { // Always receive 1 stop bit, ignore additional stop time
			owuart.state = OW_RxByte;
		} else {
			owuart.state = OW_RxWaitForNextBit;
		}
		return;
	}

	case OW_RxByte: {
		unsigned int framing_ok, byte, byte_num;
		/* Verify framing:  spddddddddS
		 *                  ^31       ^21*/
		asm("LDI %[framing_ok], 1\n\t"

			"QBNE not_ok, %[rx_bit].t31, %[stop_bit_value]\n\t"
			"LSL %[rx_bit].w2, %[rx_bit].w2, 1\n\t"

#if PARITY_ENABLED == 1
			"MOV %[byte].t8, %[rx_bit].t31\n\t"
			"LSL %[rx_bit].23, %[rx_bit].23, 1\n\t"
			"MOV %[byte].b0, %[rx_bit].b3"
#endif

			"MOV %[byte], %[rx_bit].b3\n\t"

			"QBNE not_ox, %[rx_bit].t23, %[start_bit_value]\n\t"

			"QBA framing_ok\n\t"
			"not_ok:\n\t"
			"LDI %[framing_ok], 0\n\t"

			"framing_ok:\n\t"
			: [framing_ok] "=r" (framing_ok),
			  [rx_bit] "+r" (owuart.rx_bit),
			  [byte] "=r" (byte)
			: [start_bit_value] "i" (START_BIT_VALUE),
			  [parity_mode] "i" (PARITY_MODE),
			  [stop_bit_value] "i" (STOP_BIT_VALUE)
		);

		asm("LDI %[rx_bit].b0, 0\n\t"
			: [rx_bit] "+r" (owuart.rx_bit)
		);

		if( framing_ok ) {
			// move to next byte
			asm("MOV %[byte], %[rx_bit].b1\n\t"
				"ADD %[rx_bit].b1, %[rx_bit].b1, 1\n\t"
				: [rx_bit] "+r" (owuart.rx_bit),
				  [byte] "=r" (byte_num)
			);
			owuart.rx_buf[byte_num] = byte;
			owuart.state = OW_StartHoldOff;
		} else {
			owuart.state = OW_StartHoldOff;
		}
		return;
	}

	case OW_StartHoldOff:
		timer_arm(owuart.timer, CPU_FREQ/BIT_RATE*10, -1);
		owuart.state = OW_HoldOff;
		break;

	case OW_HoldOff:
		if( timer_expired(owuart.timer) ) owuart.state = OW_EndOfMessage;
		// else stay in this state
		break;

	case OW_EndOfMessage: {
		unsigned int nbytes;
		asm("MOV %[byte], %[rx_bit].b1\n\t"
			"LDI %[rx_bit].w0, 0\n\t"
			: [byte] "=r" (nbytes),
			  [rx_bit] "+r" (owuart.rx_bit)
		);
		rpmsg_send(&transport, rpmsg_dst, rpmsg_src,
				   owuart.rx_buffer, nbytes)
				  );
		owuart.state = OW_Idle;
		return;
	}

	default:
		// TODO: handle error?
		break;
	}
}


int main() {
	// Enable the OCP master port
	// This is needed to access external memory
	PRU_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	// Interrupt setup
	// System-event-to-channel and channel-to-host mapping are done by the
	// loader based on the resource table.

	{ // Wait for linux side to indicate driver loaded completely
		volatile uint8_t *status = &rproc_resource_table.rpmsg_vdev.status;
		while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK)) ;
	}

	rpmsg_init(&transport,
		&rproc_resource_table.rpmsg_vring_pru_to_arm,
		&rproc_resource_table.rpmsg_vring_arm_to_pru,
		INTERRUPT_PRU0_VRING);

	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, "1wire UART", 0) != RPMSG_SUCCESS ) {}

	// Init state
	memset(&owuart, 0x00, sizeof(struct owuart_state));
	//owuart.timer = 0;

	// Main event loop
	while(1) process_tick();

	__builtin_unreachable();
}
