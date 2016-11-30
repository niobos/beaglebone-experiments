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
#include "lib/timer/timer.h"

#include "resource_table.h"

#define CHAN_NAME "rpmsg-pru"   // needs to match driver on Linux side

#define BIT_RATE 4800
#define MAX_MSG_SIZE 32

struct rpmsg_transport transport;
uint8_t payload[RPMSG_BUF_SIZE];

enum channel {
	CH_DATA = 'A',
	CH_STATUS = 0x0a,
};

struct owuart_state { // one-wire uart
	enum {
		OW_Idle = 0,
		OW_TxStart,
		OW_TxFindNextBit,
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
	unsigned int c_tx, c_rx, c_framing_error, c_arbitrated; // counters
	unsigned int rx_bit, tx_bit;
	unsigned int tx_buffer_len;

	unsigned int public_state_before_here[0];

	struct rpmsg_transport *transport;
	unsigned int rpmsg_src, rpmsg_dst;

	uint8_t tx_buffer[MAX_MSG_SIZE];
	uint8_t rx_buffer[MAX_MSG_SIZE];
} __attribute__((packed)) owuart[1];

void process_message(
	struct owuart_state *s,
	unsigned int src,
	unsigned int dst,
	uint8_t *msg, unsigned int len
) {
	if(len < 1) return;

	switch(msg[0]) {
	case CH_DATA: // uart channel
		if( s->tx_buffer_len ) { // Tx already queued or in progress
			// TODO: What to do in this case? Ignore? Report error?
			break;
		}
		memcpy(s->tx_buffer, msg, len);
		s->tx_buffer_len = len;
		break;

	case CH_STATUS: // status channel
		// Update src&dst cache to reply to latest received message
		s->rpmsg_src = src;
		s->rpmsg_dst = dst;
		uint8_t h = CH_STATUS;
		struct scatter msg[2] = {
			{&h, 1},
			{s, offsetof(struct owuart_state, public_state_before_here)}
		};
		rpmsg_send_gather(&transport, dst, src,
		                  msg, sizeof(msg)/sizeof(struct scatter)
		                 );
		break;

	default:
		// TODO: report error? Ignore?
		break;
	}
}

void process_tick(struct owuart_state *s) {
	switch(s->state) {
	case OW_Idle:
		break;
	case OW_TxStart:
		break;
	case OW_TxFindNextBit:
		break;
	case OW_TxBit:
		break;
	case OW_TxWaitForHalfBit:
		break;
	case OW_TxMonitorBit:
		break;
	case OW_TxWaitForFullBit:
		break;
	case OW_TxArbitrated:
		break;
	case OW_TxCompleted:
		/*rpmsg_send(&transport, s->rpmsg_dst, s->rpmsg_src,
			   &s->tx_buf_chan, s->tx_buffer_len+1
			  );
		s->tx_buffer_len = 0;*/
		break;

	case OW_RxStartBit:
		break;
	case OW_RxWaitForNextBit:
		break;
	case OW_RxBit:
		break;
	case OW_RxByte:
		break;
	case OW_RxInvalidFraming:
		break;
	case OW_StartHoldOff:
		break;
	case OW_HoldOff:
		break;
	case OW_EndOfMessage:
		break;
	default:
		// TODO: report error?
		// TODO: reset state?
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

	// initialize OW_UART's
	for(unsigned int i=0; i<sizeof(owuart)/sizeof(struct owuart_state); i++) {
		// Create a channel
		while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, "1wire UART", i) != RPMSG_SUCCESS ) {}

		// Init state
		memset(&owuart[i], 0x00, sizeof(struct owuart_state));
	}

	// Main event loop
	while(1) {
		if (read_r31() & PRU_INT_HOST0) { // Host interrupt received, process:
			// Clear the interrupt flag
			PRU_INTC.SICR = INTERRUPT_PRU_KICK;

			// Receive message(s)
			unsigned int src, dst, len;
			while(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
				process_message(&owuart[dst], src, dst, payload, len);
			}
		}

		for(unsigned int i=0; i<sizeof(owuart)/sizeof(struct owuart_state); i++) {
			process_tick(&owuart[i]);
		}
	}
}
