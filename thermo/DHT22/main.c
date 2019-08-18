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

struct rpmsg_transport transport;

uint8_t payload[RPMSG_BUF_SIZE];

#define NUM_SENSORS 4
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
		DHT22_Done
	} state;
	unsigned int rx_bit;
	unsigned int timer;
} DHT22[NUM_SENSORS];

unsigned int map_gpio(unsigned int output) {
    /* Maps output number to GPIO bit mask.
     * Returns 0 on error.
     */
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
	//GPIO2.SETDATAOUT = bit;
	GPIO2.OE |= bit;
}

unsigned int read_input(unsigned int input) {
	unsigned int bit = map_gpio(input);
	return (GPIO2.DATAIN & bit) != 0;
}

void store_next_bit(unsigned int *pointer, void *buf, unsigned int bit_value) {
    /* Store the bit `bit_value` (which should be either 0 or 1) in the buffer
     * pointed to by `*buf` at position specified by the opaque value pointed
     * to by `*pointer`. A `*pointer` value of 0 writes the most significant bit
     * of the first byte (i.e. big-endian).
     * `*pointer` will be incremented after this operation.
     */
	unsigned int b;
	asm volatile ( // volatile: this block has side effects (changing *buf)
		"MOV %[b].b1, %[p].b0\n\t"
		"RSB %[b].b1, %[b].b1, 7\n\t" // most significant bit first, so bit (7-N)

		"LSL %[v], %[v], %[b].b1\n\t" // left-shift bit in position
		"LBBO %[b].b0, %[buf], %[p].b1, 1\n\t" // Load buf[byte]
		"OR %[b].b0, %[b].b0, %[v].b0\n\t"
		"SBBO %[b].b0, %[buf], %[p].b1, 1\n\t" // store buf[byte]

		"ADD %[p].b0, %[p].b0, 1\n\t"
		"QBGT store_next_bit__not_next_byte, %[p].b0, 8\n\t" // branch if 8 > bitnumber
		"ADD %[p].b1, %[p].b1, 1\n\t"
		"LDI %[p].b0, 0\n\t"
		"store_next_bit__not_next_byte:\n\t"
		: /* output */
			[p] "+&r" (*pointer),
			[b] "=&r" (b)
		: /* input */
			[buf] "r" (buf),
			[v] "r" (bit_value)
		: /* clobber */
			"memory" // *buf is changed
	);
}

void do_measurements() {
	unsigned int done = 0;
	memset(DHT22, 0x00, sizeof(DHT22));

	while( ! done ) {
		done = 1;

		for( unsigned int i = 0; i < NUM_SENSORS; i++ ) {
			done = done && (DHT22[i].state == DHT22_Done);

			switch( DHT22[i].state ) {
			case DHT22_SendStart:
				write_low(i);
				DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * 500;  // 500us
				DHT22[i].state = DHT22_WaitForStartEnd;
				break;

			case DHT22_WaitForStartEnd:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					write_hiz(i);
					DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * (40+4);  // 40us + 10%
					DHT22[i].state = DHT22_WaitForSensorResponse;
				}
				break;

			case DHT22_WaitForSensorResponse:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].state |= 0x80;  // Go to failure state
				}
				if( read_input(i) == 0 ) {
					DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * (80+8);  // 80us + 10%
					DHT22[i].state = DHT22_WaitForSensorResponseEnd;
				}
				break;

			case DHT22_WaitForSensorResponseEnd:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].state |= 0x80;  // Go to failure state
				}
				if( read_input(i) == 1 ) {
					DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * (80+8);  // 80us + 10%
					DHT22[i].state = DHT22_WaitForBit;
				}
				break;

			case DHT22_WaitForBit:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].state |= 0x80;  // Go to failure state
				}
				if( read_input(i) == 0 ) {
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			case DHT22_BitStart:
				DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * (50+50); // 50µs+10% was not enough is some cases
				DHT22[i].state = DHT22_WaitForBitRelease;
				break;

			case DHT22_WaitForBitRelease:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].state |= 0x80;  // Go to failure state
				}
				if( read_input(i) == 1 ) {
					if( DHT22[i].rx_bit >= 0x0500 ) {
						DHT22[i].state = DHT22_Done;
					} else {
						DHT22[i].timer = time_get() + CPU_FREQ / 1000000 * 50; // 50µs is decision point between "0" and "1"
						DHT22[i].state = DHT22_ReadBit0;
					}
				}
				break;

			case DHT22_ReadBit0:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].timer += CPU_FREQ / 1000000 * (20+7); // extend timer, don't re-arm
					DHT22[i].state = DHT22_ReadBit1;
				}
				if( read_input(i) == 0 ) {
					store_next_bit(&DHT22[i].rx_bit, payload + i*5, 0);
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			case DHT22_ReadBit1:
				if( time_ge(time_get(), DHT22[i].timer) ) {
					DHT22[i].state |= 0x80;  // Go to failure state
				}
				if( read_input(i) == 0 ) {
					store_next_bit(&DHT22[i].rx_bit, payload + i*5, 1);
					DHT22[i].state = DHT22_BitStart;
				}
				break;

			default:
				write_hiz(i);
				*(payload + i*5 + 0) = 0xff;
				*(payload + i*5 + 1) = DHT22[i].state;
				*(payload + i*5 + 2) = DHT22[i].rx_bit;
				*(payload + i*5 + 3) = DHT22[i].rx_bit >> 8;
				*(payload + i*5 + 4) = 0xfe;
				/* Checksum is guaranteed to fail:
				 * rx_bit&0xff is <=7; rx_bit>>8 is <= 4
				 * state is < 0x8a
				 * Also: humidity of more than 6528.0% should rise suspicion
				 */
				DHT22[i].state = DHT22_Done;
				// fall through
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
		INTERRUPT_PRU_VRING
	);

	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME,
	                     "environment sensors", 10*PRU_NUMBER) != RPMSG_SUCCESS ) {}

	// Main event loop
	while(1) {
		unsigned int src, dst, len;
		if(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
			memset(payload, 0x00, NUM_SENSORS*5);
			do_measurements();
			rpmsg_send(&transport, dst, src, payload, NUM_SENSORS*5);
			__delay_cycles(CPU_FREQ * 2); // Wait 2 seconds before next measurement can be taken
		}
		/* Potential BUG
		 * time_get() might not be called within 21 seconds while waiting for an
		 * rpmsg.
		 * This is currently fine, since no timers are actually running
		 */
	}
	__builtin_unreachable();
}
