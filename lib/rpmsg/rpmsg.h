#pragma once

#include <linux/rpmsg.h>
#include <linux/virtio_ring.h>
#include <linux/rsc_types.h>

struct virtqueue {
	unsigned int  id;
	unsigned int  sys_event;
	unsigned int  last_avail_idx;
	struct vring  vring;
};

void virtqueue_init (
	struct virtqueue          *vq,
	struct fw_rsc_vdev_vring  *vring,
	volatile unsigned int     sys_event
);

unsigned int virtqueue_get_avail_buf (
	struct virtqueue  *vq,
	void              **buf,
	unsigned int      *len
);
#define VIRTQUEUE_NO_BUF_AVAILABLE 0xffffff03

unsigned int virtqueue_add_used_buf (
	struct virtqueue  *vq,
	unsigned int      head,
	unsigned int      len
);
#define VIRTQUEUE_INVALID_HEAD 0xffffff04
#define VIRTQUEUE_SUCCESS 0

unsigned int virtqueue_kick (
	struct virtqueue *vq
);
#define VIRTQUEUE_NO_KICK 0xffffff05



struct rpmsg_transport {
	struct virtqueue pru_to_arm;
	struct virtqueue arm_to_pru;
};

#define RPMSG_BUFFER_TOO_SMALL    0xffffff00
#define RPMSG_NO_BUFFER_AVAILABLE 0xffffff01
#define RPMSG_INVALID_HEAD        0xffffff02
#define RPMSG_SUCCESS             0

struct scatter {
	void          *data;
	unsigned int  len;
};

unsigned int rpmsg_init(
	struct rpmsg_transport *transport,
	struct fw_rsc_vdev_vring *vring_pru_to_arm,
	struct fw_rsc_vdev_vring *vring_arm_to_pru,
	unsigned int sys_event_to_arm
);
/* Initialize an RPMsg transport for usage.
 * Needs to be called once per transport (i.e. once per PRU), usually like this:
 * rpmsg_init(&transport,  // struct to be initialized
 *     &rproc_resource_table.rpmsg_vring_pru_to_arm,
 *     &rproc_resource_table.rpmsg_vring_arm_to_pru,
 *     // ^^ data filled in by kernel, available after PRU boots
 *     INTERRUPT_PRU_VRING  // define in the device tree, or in <linux/pru.h>
 * );
 */

unsigned int rpmsg_channel(
	enum rpmsg_ns_flags     flags,
	struct rpmsg_transport  *transport,
	char                    *name,
	char                    *desc,
	unsigned int            port
);
/* Register a channel of communication
 * name must match the name of the driver on the linux-side of the channel.
 * Set to "rpmsg-pru" to use the /dev/rpmsg_pruN interface.
 * port is an arbitrary port number for this channel. Multiple channels can
 * be registered on the same transport, using separate ports to differentiate.
 * When using the "rpmsg-pru" driver, the port maps to number in the device
 * name, and messages to different devices will have the corresponding port
 * as dst-field.
 * Note that messages for different ports are sent interleaved, and not queueed
 * individually.
 */

unsigned int rpmsg_send (
	struct rpmsg_transport  *transport,
	unsigned int            src,
	unsigned int            dst,
	void                    *data,
	unsigned int            len
);
unsigned int rpmsg_send_gather (
	struct rpmsg_transport  *transport,
	unsigned int            src,
	unsigned int            dst,
	struct scatter          data[],
	unsigned int            count
);

unsigned int rpmsg_receive (
	struct rpmsg_transport  *transport,
	unsigned int            *src,
	unsigned int            *dst,
	void                    *data,
	unsigned int            *len
);
