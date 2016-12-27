#include <pru/constant_table.h>
#include <pru/cfg.h>
#include <pru/intc.h>
#include <am335x/gpio.h>
#include <linux/pru.h>
#include <linux/virtio_config.h>
#include <pru/io.h>
#include <pru/ctrl.h>

#include <string.h> // memcpy()

#include "../lib/rpmsg/rpmsg.h"
#include "../lib/timer/timer.h"

#include "resource_table.h"

#define CHAN_NAME "rpmsg-pru"   // needs to match driver on Linux side

struct rpmsg_transport transport;

uint8_t payload[RPMSG_BUF_SIZE];

struct DHT22 {
	enum dht22_state {
		DHT22_SendStart = 0,
		DHT22_WaitForStartEnd,
		DHT22_WaitForSensorResponse,
		DHT22_WaitForSensorResponseEnd,
		DHT22_WaitForBit,
		DHT22_BitStart,
		DHT22_WaitForBitRelease,
		DHT22_ReadBit0,
		DHT22_ReadBit1,
		DHT22_Done = 0xf0,
		DHT22_Failed = 0xf1
	} state;
	unsigned int rx_bit;
} DHT22[4];

unsigned int map_gpio(unsigned int output) {
	switch(output) {
		case 0: return 1<<2;
		case 1: return 1<<3;
		case 2: return 1<<5;
		case 3: return 1<<4;
		default: return 0;
	}
}

void write_low(unsigned int output) {
	unsigned int bit = map_gpio(output);
	GPIO2.CLEARDATAOUT = bit;
	GPIO2.OE &= ~bit;
}

void write_hiz(unsigned int output) {
	unsigned int bit = map_gpio(output);
	GPIO2.SETDATAOUT = bit;
	GPIO2.OE |= bit;
}

unsigned int read_input(unsigned int input) {
	unsigned int bit = map_gpio(input);
	return !!(GPIO2.DATAIN & bit);
}

unsigned int toggle;
void store_next_bit(unsigned int *pointer, void *buf, unsigned int bit_value) {
	unsigned int b;
	asm volatile (
		"MOV %[b].b1, %[p].b0\n\t"
		"RSB %[b].b1, %[b].b1, 7\n\t" // most significant bit first, so bit (7-N)

		"LSL %[v], %[v], %[b].b1\n\t" // left-shift bit in position
		"LBBO %[b].b0, %[buf], %[p].b1, 1\n\t" // Load buf[byte]
		"OR %[b].b0, %[b].b0, %[v].b0\n\t"
		"SBBO %[b].b0, %[buf], %[p].b1, 1\n\t" // store buf[byte]

		"ADD %[p].b0, %[p].b0, 1\n\t"
		"QBGT store_next_bit_not_next_byte, %[p].b0, 8\n\t" // 8 > bitnumber
		"ADD %[p].b1, %[p].b1, 1\n\t"
		"LDI %[p].b0, 0\n\t"
		"store_next_bit_not_next_byte:\n\t"
		: /* output */
			[p] "+&r" (*pointer),
			[b] "=&r" (b)
		: /* input */
			[buf] "r" (buf),
			[v] "r" (bit_value)
		: /* clobber */
			"memory"
	);
	if( toggle ) write_r30_set_bit(14);
	else write_r30_clr_bit(14);
	toggle = !toggle;
	if( bit_value ) write_r30_set_bit(15);
	else write_r30_clr_bit(15);
}

void do_measurements() {
	unsigned int done = 0;
	memset(DHT22, 0x00, sizeof(DHT22));

	while( ! done ) {
		done = (DHT22[0].state >= 0xf0) &&
		       (DHT22[1].state >= 0xf0) &&
		       (DHT22[2].state >= 0xf0);

		for( unsigned int i = 0; i <= 3; i++ ) {
			switch( DHT22[i].state ) {
			case DHT22_SendStart:
				write_r30_set_bit(14);
				timer_arm(i, CPU_FREQ / 1000000 * 500, 0);
				write_low(i);
				DHT22[i].state = DHT22_WaitForStartEnd;
				break;

			case DHT22_WaitForStartEnd:
				if( timer_expired(i) ) {
					write_hiz(i);
					timer_arm(i, CPU_FREQ / 1000000 * (40+4), 0);
					DHT22[i].state = DHT22_WaitForSensorResponse;
				}
				break;

			case DHT22_WaitForSensorResponse:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_Failed;
				}
				if( read_input(i) == 0 ) {
					timer_arm(i, CPU_FREQ / 1000000 * (80+8), 0);
					DHT22[i].state = DHT22_WaitForSensorResponseEnd;
				}
				break;

			case DHT22_WaitForSensorResponseEnd:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_Failed;
				}
				if( read_input(i) == 1 ) {
					timer_arm(i, CPU_FREQ / 1000000 * (80+8), 0);
					DHT22[i].state = DHT22_WaitForBit;
				}
				break;

			case DHT22_WaitForBit:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_Failed;
				}
				if( read_input(i) == 0 ) {
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			case DHT22_BitStart:
				timer_arm(i, CPU_FREQ / 1000000 * (50+50), 0);
				DHT22[i].state = DHT22_WaitForBitRelease;
				break;

			case DHT22_WaitForBitRelease:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_Failed;
				}
				if( read_input(i) == 1 ) {
					if( DHT22[i].rx_bit >= 0x0500 ) {
						timer_disable(i);
						DHT22[i].state = DHT22_Done;
						write_r30_clr_bit(14);
					} else {
						timer_arm(i, CPU_FREQ / 1000000 * 50, CPU_FREQ / 1000000 * (20+7));
						DHT22[i].state = DHT22_ReadBit0;
					}
				}
				break;

			case DHT22_ReadBit0:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_ReadBit1;
				}
				if( read_input(i) == 0 ) {
					store_next_bit(&DHT22[i].rx_bit, payload + i*5, 0);
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			case DHT22_ReadBit1:
				if( timer_expired(i) ) {
					DHT22[i].state = DHT22_Failed;
				}
				if( read_input(i) == 0 ) {
					store_next_bit(&DHT22[i].rx_bit, payload + i*5, 1);
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			case DHT22_Failed:
				write_hiz(i);
				timer_disable(i);
				*(payload + i*5 + 4) = 0xff;
				break;

			case DHT22_Done:
				break;
			}
		}
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

	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, "environment sensors", 0) != RPMSG_SUCCESS ) {}

	// Main event loop
	while(1) {
		unsigned int src, dst, len;
		if(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
			memset(payload, 0x00, 4*5);
			do_measurements();
			rpmsg_send(&transport, dst, src, payload, 4*5);
		}
	}
	__builtin_unreachable();
}
