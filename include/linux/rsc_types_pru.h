#pragma once

#include "rsc_types.h"

#define TYPE_PRU_INTS 1

/**
 * struct ch_map - sysevts-to-channel mapping
 *
 * @evt: the number of the sysevt
 * @ch: channel number assigned to a given @sysevt
 *
 * PRU system events are mapped to channels, and these channels are mapped to
 * hosts. Events can be mapped to channels in a one-to-one or many-to-one ratio
 * (multiple events per channel), and channels can be mapped to hosts in a
 * one-to-one or many-to-one ratio (multiple events per channel).
 *
 * @evt is the number of the sysevt, and @ch is the number of the channel to be
 * mapped.
 */

struct ch_map {
	uint8_t evt;
	uint8_t ch;
};

/**
 * struct fw_rsc_custom_ints - custom resource to define PRU interrupts
 * @version: revision number of the custom ints type
 * @channel_host: assignment of PRU channels to hosts
 * @num_evts: device address of INTC
 * @event_channel: mapping of sysevts to channels
 *
 * PRU system events are mapped to channels, and these channels are mapped to
 * hosts. Events can be mapped to channels in a one-to-one or many-to-one ratio
 * (multiple events per channel), and channels can be mapped to hosts in a
 * one-to-one or many-to-one ratio (multiple events per channel).
 *
 * @da is the device address of the interrupt controller, @channel_map is
 * used to specify to which channel, if any, an event is mapped, and @host_map
 * specifies to which host, if any, a channel is mapped.
 */
struct fw_rsc_custom_ints {
	uint16_t version;
	uint8_t channel_host[10];
	uint32_t num_evts;
	struct ch_map *event_channel;
};
