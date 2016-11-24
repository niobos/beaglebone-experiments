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
#define CHAN_DESC "Channel 30"
#define CHAN_PORT 30

uint8_t payload[RPMSG_BUF_SIZE];

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

	struct rpmsg_transport transport;
	rpmsg_init(&transport,
		&rproc_resource_table.rpmsg_vring_pru_to_arm,
		&rproc_resource_table.rpmsg_vring_arm_to_pru,
		INTERRUPT_PRU0_VRING);

	// Clear the interrupt status to start clean
	PRU_INTC.SICR = INTERRUPT_PRU_KICK;

	// Create a channel
	while( rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != RPMSG_SUCCESS ) {}

	timer_arm(0, 200000000, 200000000); // 1 second

	// Main event loop
	while(1) {
		if (read_r31() & PRU_INT_HOST0) { // Host interrupt received, process:
			// Clear the interrupt flag
			PRU_INTC.SICR = INTERRUPT_PRU_KICK;

			unsigned int src, dst, len;
			// Receive message(s)
			while(rpmsg_receive(&transport, &src, &dst, payload, &len) == RPMSG_SUCCESS) {
				unsigned int a = timer_expired(0);
				rpmsg_send(&transport, dst, src, &a, 4);
			}
		}
	}

}
