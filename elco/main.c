#include <pru/constant_table.h>
#include <pru/cfg.h>
#include <pru/intc.h>
#include <am335x/gpio.h>
#include <linux/pru.h>
#include <linux/virtio_config.h>
#include <pru/id.h>
#include <pru/io.h>
#include <pru/ctrl.h>

#include <string.h> // memcpy()

#include "../lib/rpmsg/rpmsg.h"
#include "../lib/time/time.h"

#include "resource_table.h"

#define CHAN_NAME "rpmsg-pru"   // needs to match driver on Linux side
#define MAIN_CHAN_NUM (10*PRU_NUMBER+0)
#define AUX_CHAN_NUM (10*PRU_NUMBER+1)

#define MAX_MSG_SIZE 32
#define BIT_RATE 4800
#define START_BIT_VALUE 1
#define PARITY_MODE 0 // 0=even, 1=odd
#define STOP_BIT_VALUE 0

struct rpmsg_transport transport;
unsigned int rpmsg_src;

uint8_t payload[RPMSG_BUF_SIZE];

struct owuart_state { // one-wire uart
	enum {
		OW_Idle = 0,
		OW_TxNextBit,
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
		OW_RxInvalidFraming,
		OW_StartHoldOff,
		OW_HoldOff,
		OW_EndOfMessage
	} state;
	unsigned int timer;

	unsigned int c_tx, c_rx, c_arbitrated, c_framing_error; // counters

	unsigned int rx_bit, tx_bit;
	unsigned int tx_buffer_len;
	uint8_t tx_buffer[MAX_MSG_SIZE];
	uint8_t rx_buffer[MAX_MSG_SIZE];
} owuart;

unsigned int read() {
	return (GPIO1.DATAIN & (1<<14)) >> 14;
}
void write(unsigned int b) {
	if( b ) GPIO1.SETDATAOUT = 1<<15;
	else GPIO1.CLEARDATAOUT = 1<<15;
}

unsigned int parity(unsigned int data) { // Assume 1 byte of data
	unsigned int mode = PARITY_MODE;
	asm("LSR %[data].b1, %[data].b0, 4\n\t"
		"XOR %[data].b0, %[data].b0, %[data].b1\n\t"
		"LSR %[data].b1, %[data].b0, 2\n\t"
		"XOR %[data].b0, %[data].b0, %[data].b1\n\t"
		"LSR %[data].b1, %[data].b0, 1\n\t"
		"XOR %[data].b0, %[data].b0, %[data].b1\n\t"

		"XOR %[parity].b0, %[parity].b0, %[data].b0\n\t"
		"AND %[parity], %[parity], 0x01\n\t"
		: /* outputs */
			[parity] "+r" (mode)
		: /* inputs */
			[data] "r" (data)
		: /* clobber */
	);
	return mode;
}

void process_tick() {
	switch(owuart.state) {
	case OW_Idle: {
		if( read() == 1 ) {
			owuart.state = OW_RxStartBit;
			return;
		}

		if( owuart.tx_buffer_len ) {
			owuart.tx_bit = 0x00fe;  // the bit before the first start bit
			owuart.state = OW_TxNextBit;
			return;
		}

		unsigned int src, dst, len;
		if(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
			if( dst == MAIN_CHAN_NUM ) {
				rpmsg_src = src;
				if(len > MAX_MSG_SIZE) return;  // TODO: report error?
				memcpy(owuart.tx_buffer, payload, len);
				owuart.tx_buffer_len = len;
				// remain in idle state, will catch this on next tick;
			} else if( dst == AUX_CHAN_NUM ) {
				rpmsg_send(&transport, AUX_CHAN_NUM, src,
				           &owuart.c_tx, 4*4);
			}
		}

		return;
	}

	case OW_TxNextBit: {
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
			: /* inputs */ [nbits] "i" (8+1+1+1) // databits + parity + stop bit + 1
			: /* clobber */
		);
		if( byte >= owuart.tx_buffer_len) owuart.state = OW_TxCompleted;
		else owuart.state = OW_TxBit;
		owuart.timer = time_get();
		return;
	}

	case OW_TxBit: {
		unsigned int bit_to_tx, bitnum_to_tx;
		// Load byte to transmit
		asm(
			"LBBO %[bit_to_tx], %[buf], %[tx_bit].b1, 1\n\t"
			"MOV %[bitnum_to_tx], %[tx_bit].b0\n\t"
			: /* outputs */
				[bit_to_tx] "=r" (bit_to_tx),
				[bitnum_to_tx] "=r" (bitnum_to_tx),
				[tx_bit] "+r" (owuart.tx_bit)
			: /* inputs */
				[buf] "r" (owuart.tx_buffer)
			: /* clobber */
		);

		if( bitnum_to_tx == 0xff ) {
			// Tx start bit
			bit_to_tx = START_BIT_VALUE;
		} else if( bitnum_to_tx < 8 ) {
			// Tx data bits
			bit_to_tx >>= bitnum_to_tx;
			bit_to_tx &= 0x01;
		} else if( bitnum_to_tx == 8 ) {
			bit_to_tx = parity(bit_to_tx);
		} else {
			bit_to_tx = STOP_BIT_VALUE;
		}
		write(bit_to_tx);

		asm(
			"MOV %[tx_bit].b3, %[bit_to_tx].b0\n\t"
			: /* outputs */
				[tx_bit] "+r" (owuart.tx_bit)
			: /* inputs */
				[bit_to_tx] "r" (bit_to_tx)
			: /* clobber */
		);
		owuart.timer += CPU_FREQ / BIT_RATE / 2;
		owuart.state = OW_TxWaitForHalfBit;
		return;
	}

	case OW_TxWaitForHalfBit:
		if( time_ge(time_get(), owuart.timer) ) owuart.state = OW_TxMonitorBit;
		// else stay in this state
		return;

	case OW_TxMonitorBit: {
		unsigned int bit;
		asm("MOV %[bit], %[tx_bit].b3\n\t"
			: /* outputs */ [bit] "=r" (bit)
			: /* inputs */  [tx_bit] "r" (owuart.tx_bit)
			: /* clobber */
		);
		if( read() == bit ) {
			owuart.state = OW_TxWaitForFullBit;
		} else {
			owuart.state = OW_TxArbitrated;
		}
		owuart.timer += CPU_FREQ / BIT_RATE / 2;
		return;
	}

	case OW_TxWaitForFullBit:
		if( time_ge(time_get(), owuart.timer) ) owuart.state = OW_TxNextBit;
		return;

	case OW_TxArbitrated: {
		// Someone else was sending on the bus while we were as well
		// Either they started (almost) at the same time:
		//   In that case, we actually received the same bits we sent so far
		//   Copy over the Tx buffer in to the Rx buffer
		//   Jump to Rx-code, to re-read this bit (since this is the
		//   first bit to mismatch)
		// Or someone is not respecting the protocol and simply breaking in
		// to our transmission
		//   In that case, we have no idea what the first bit send would be
		//   Ignore this case and copy over everything as well. Hopefully
		//   the parity and/or CRC will cover this

		write(0); // set Hi-Z
		owuart.c_arbitrated++;

		// Get current tx byte
		unsigned int byte;
		asm(
			"LBBO %[byte], %[tx_buf], %[tx_bit].b1, 1\n\t"  // load Tx byte
			: /* outputs */
				[byte] "=r" (byte)
			: /* inputs */
				[tx_bit] "r" (owuart.tx_bit),
				[tx_buf] "r" (owuart.tx_buffer)
			: /* clobber */
		);

		asm(//fill in Rx bit buffer with constructed bits
			"MOV %[rx_bit].w2, %[v]\n\t"
			: /* outputs */
				[rx_bit] "+r" (owuart.rx_bit)
			: /* inputs */
				[v] "r" (
					STOP_BIT_VALUE << 15 |
					parity(byte) << 14 |
					byte << 6 |
					START_BIT_VALUE << 5)
		);

		asm(// Now shift away bits still to be received
			// tx bit 0 is the 0th data bit (start bit is 0xff)
			// tx bit 9 is stop bit
			"RSB %[num_bytes], %[tx_bit].b0, 10\n\t"  // 10-tx_bit
			"LSL %[rx_bit].w2, %[rx_bit].w2, %[num_bytes]\n\t"

			// Copy over tx_state in to rx_state
			"ADD %[rx_bit].b0, %[tx_bit].b0, 1\n\t" // rx counts start-bit as bit0, tx as bit 0xff
			"MOV %[rx_bit].b1, %[tx_bit].b1\n\t"
			"MOV %[num_bytes], %[rx_bit].b1\n\t"
			: /* outputs */
				[rx_bit] "+r" (owuart.rx_bit),
				[num_bytes] "=r" (byte)
			: /* inputs */
				[tx_bit] "r" (owuart.tx_bit)
			: /* clobber */
		);
		memcpy(owuart.rx_buffer, owuart.tx_buffer, byte);
		owuart.state = OW_RxBit;
		return;
	}

	case OW_TxCompleted:
		rpmsg_send(&transport, MAIN_CHAN_NUM, rpmsg_src,
		           owuart.tx_buffer, owuart.tx_buffer_len);
		write(0); // To be sure. Stop-bit should have already done this
		owuart.tx_buffer_len = 0;
		owuart.state = OW_StartHoldOff;
		owuart.c_tx++;
		return;

	case OW_RxStartBit:
		owuart.timer = time_get() + CPU_FREQ / BIT_RATE / 2;
		owuart.state = OW_RxWaitForNextBit;
		return;

	case OW_RxWaitForNextBit:
		if( time_ge(time_get(), owuart.timer) ) owuart.state = OW_RxBit;
		return;

	case OW_RxBit: {
		unsigned int bit;
		asm(/* rx_bit is a bit-struct:
			 * bits 31-16: received bits (aligned left)
			 * bits 15-8: byte
			 * bits 7-0: bit
			 */
			"LSR %[rx_bit].w2, %[rx_bit].w2, 1\n\t"
			"QBNE not_one, %[in], 1\n\t"
			"SET %[rx_bit], %[rx_bit], 31\n\t"
			"not_one:\n\t"

			"ADD %[rx_bit].b0, %[rx_bit].b0, 1\n\t"
			"MOV %[bit], %[rx_bit].b0\n\t"
			: /* outputs */ [rx_bit] "+r" (owuart.rx_bit),
			[bit] "=r" (bit)
			: /* inputs */ [in] "r" (read())
			: /* clobber */
		);
		if( bit >= 1+8+1+1 ) { // Always receive 1 stop bit, ignore additional stop time
			owuart.state = OW_RxByte;
		} else {
			owuart.state = OW_RxWaitForNextBit;
			owuart.timer += CPU_FREQ / BIT_RATE;
		}
		return;
	}

	case OW_RxByte: {
		unsigned int framing_ok, byte, byte_num;
		/* Verify framing:  spddddddddS
		 *                  ^31       ^21*/
		framing_ok = 1;
		if( (owuart.rx_bit & 0x00200000) != (START_BIT_VALUE<<21) ) framing_ok = 0;
		else if( (owuart.rx_bit & 0x80000000) != (STOP_BIT_VALUE<<31) ) framing_ok = 0;

		asm("LSR %[rx_bit].w2, %[rx_bit].w2, 6\n\t"
			"MOV %[byte], %[rx_bit].b2\n\t"
			// Parity is in rx_bit & 0x01000000
			: /* outputs */
				[rx_bit] "+r" (owuart.rx_bit),
				[byte] "=r" (byte)
			: /* inputs */
			: /* clobber */
		);

		if( (owuart.rx_bit & 0x01000000)>>24 != parity(byte) ) framing_ok = 0;

		asm("LDI %[rx_bit].b0, 0\n\t"
			: /* output */
				[rx_bit] "+r" (owuart.rx_bit)
		);

		if( framing_ok ) {
			// move to next byte
			asm("MOV %[byte_num], %[rx_bit].b1\n\t"
				"ADD %[rx_bit].b1, %[rx_bit].b1, 1\n\t"
				: [rx_bit] "+r" (owuart.rx_bit),
				  [byte_num] "=r" (byte_num)
			);
			owuart.rx_buffer[byte_num] = byte;
			owuart.state = OW_StartHoldOff;
		} else {
			owuart.state = OW_StartHoldOff;
			owuart.c_framing_error++;
		}
		return;
	}

	case OW_StartHoldOff:
		owuart.timer = time_get() + CPU_FREQ / BIT_RATE * 11; // wait 1 full character
		owuart.state = OW_HoldOff;
		return;

	case OW_HoldOff:
		if( read() == 1 ) {
			owuart.state = OW_RxStartBit;
		} else if( time_ge(time_get(), owuart.timer) ) {
			owuart.state = OW_EndOfMessage;
		}
		return;

	case OW_EndOfMessage: {
		unsigned int nbytes;
		asm("MOV %[byte], %[rx_bit].b1\n\t"
			"LDI %[rx_bit].w0, 0\n\t"
			: [byte] "=r" (nbytes),
			  [rx_bit] "+r" (owuart.rx_bit)
		);
		if( nbytes ) {
			rpmsg_send(&transport, MAIN_CHAN_NUM, rpmsg_src,
			           owuart.rx_buffer, nbytes);
			owuart.c_rx++;
		}
		owuart.state = OW_Idle;
		return;
	}

	default:
		// TODO: handle error?
		write(0);
		owuart.state = OW_StartHoldOff;
		return;
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
		while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK)) {}
	}

	rpmsg_init(&transport,
		&rproc_resource_table.rpmsg_vring_pru_to_arm,
		&rproc_resource_table.rpmsg_vring_arm_to_pru,
		INTERRUPT_PRU_VRING);

	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME,
	                     "1wire UART", MAIN_CHAN_NUM) != RPMSG_SUCCESS ) {}
	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME,
	                     "1wire UART AUX", AUX_CHAN_NUM) != RPMSG_SUCCESS ) {}

	// Init state
	memset(&owuart, 0x00, sizeof(struct owuart_state));

	// Main event loop
	while(1) process_tick();

	__builtin_unreachable();
}
