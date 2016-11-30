#include "rpmsg.h"
#include <pru/io.h>

#include <string.h> // memcpy()

/* bit 5 is the valid strobe to generate system events with R31 */
#define INT_ENABLE  (1 << 5)
/* R31[3:0] can generate 15-0 which maps to system events 31-16
 * e.g. to generate PRU-ICSS System Event 17 (pru_mst_intr[1])
 * R31 = (INT_ENABLE | (17 - INT_OFFSET));
 */
#define INT_OFFSET  16

void virtqueue_init (
	struct virtqueue          *vq,
	struct fw_rsc_vdev_vring  *vring,
	volatile unsigned int     sys_event
) {
	vq->id = vring->notifyid;
	vq->sys_event = sys_event;
	vq->last_avail_idx = 0;

	vring_init(&vq->vring, vring->num, (void*)vring->da, vring->align);
}

unsigned int virtqueue_get_avail_buf (
	struct virtqueue  *vq,
	void              **buf,
	unsigned int      *len
) {
	struct vring_avail *avail = vq->vring.avail;

	/* There's nothing available */
	if (vq->last_avail_idx == avail->idx) {
		return VIRTQUEUE_NO_BUF_AVAILABLE;
	}

	/*
	 * Grab the next descriptor number the ARM host is advertising, and
	 * increment the last available index we've seen.
	 */
	unsigned int head = avail->ring[vq->last_avail_idx++ & (vq->vring.num - 1)];

	struct vring_desc desc = vq->vring.desc[head];
	*buf = (void *)(uint32_t)desc.addr;
	*len = desc.len;

	return head;
}

unsigned int virtqueue_add_used_buf (
	struct virtqueue  *vq,
	unsigned int      head,
	unsigned int      len
) {
	unsigned int num = vq->vring.num;
	struct vring_used *used = vq->vring.used;

	if (head > num)
		return VIRTQUEUE_INVALID_HEAD;

	/*
	 * The virtqueue's vring contains a ring of used buffers.  Get a pointer to
	 * the next entry in that used ring.
	 */
	struct vring_used_elem *used_elem = &used->ring[used->idx++ & (num - 1)];
	used_elem->id = head;
	used_elem->len = len;

	return VIRTQUEUE_SUCCESS;
}

unsigned int virtqueue_kick (
	struct virtqueue *vq
) {
	/* If requested, do not kick the ARM host */
	if (vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT)
		return VIRTQUEUE_NO_KICK;

	/* Generate a system event to kick the ARM */
	write_r31( INT_ENABLE | (vq->sys_event - INT_OFFSET) );

	return VIRTQUEUE_SUCCESS;
}



unsigned int rpmsg_init(
	struct rpmsg_transport *transport,
	struct fw_rsc_vdev_vring *vring_pru_to_arm,
	struct fw_rsc_vdev_vring *vring_arm_to_pru,
	unsigned int sys_event_to_arm
) {
	virtqueue_init(&transport->pru_to_arm,
		vring_pru_to_arm, sys_event_to_arm);
	virtqueue_init(&transport->arm_to_pru,
		vring_arm_to_pru, 0);

	return RPMSG_SUCCESS;
}

unsigned int rpmsg_channel(
	enum rpmsg_ns_flags     flags,
	struct rpmsg_transport  *transport,
	char                    *name,
	char                    *desc,
	unsigned int            port
) {
	struct rpmsg_ns_msg ns_msg;

	for(unsigned int i = 0; i < RPMSG_NAME_SIZE; i++){
		ns_msg.name[i] = name[i];
		if( name[i] == '\0' ) break;
	}
	for(unsigned int i = 0; i < RPMSG_NAME_SIZE; i++){
		ns_msg.desc[i] = desc[i];
		if( desc[i] == '\0' ) break;
	}
	ns_msg.addr = port;
	ns_msg.flags = flags;

	return rpmsg_send(transport, port, 53, &ns_msg, sizeof(ns_msg));
}

unsigned int rpmsg_send (
	struct rpmsg_transport  *transport,
	unsigned int            src,
	unsigned int            dst,
	void                    *data,
	unsigned int            len
) {
	struct scatter s;
	s.data = data;
	s.len = len;
	return rpmsg_send_gather(transport, src, dst, &s, 1);
}

unsigned int rpmsg_send_gather (
	struct rpmsg_transport  *transport,
	unsigned int            src,
	unsigned int            dst,
	struct scatter          data[],
	unsigned int            count
) {
	unsigned int total_len = 0;
	for(unsigned int i=0; i < count; i++) {
		total_len += data[i].len;
	}

	if(total_len > (RPMSG_BUF_SIZE - sizeof(struct rpmsg_hdr))) {
		return RPMSG_BUFFER_TOO_SMALL;
	}

	struct virtqueue *virtqueue = &transport->pru_to_arm;

	/* Get an available buffer */
	struct rpmsg_hdr *msg;
	unsigned int msg_len;
	unsigned int head = virtqueue_get_avail_buf(virtqueue, (void **)&msg, &msg_len);

	if(head >= 0xffffff00) {
		return RPMSG_NO_BUFFER_AVAILABLE;
	}

	/* Copy local data buffer to the descriptor buffer address */
	msg->dst = dst;
	msg->src = src;
	msg->flags = 0;
	msg->reserved = 0;
	void *p = msg->data;
	for(unsigned int i=0; i < count; i++) {
		memcpy(p, data[i].data, data[i].len);
		p += data[i].len;
	}
	msg->len = total_len;

	/* Add the used buffer */
	if(virtqueue_add_used_buf(virtqueue, head, msg_len) >= 0xffffff00) {
		return RPMSG_INVALID_HEAD;
	}

	/* Kick the ARM host */
	virtqueue_kick(virtqueue);

	return RPMSG_SUCCESS;
}

unsigned int rpmsg_receive (
	struct rpmsg_transport  *transport,
	unsigned int            *src,
	unsigned int            *dst,
	void                    *data,
	unsigned int            *len
) {
	struct virtqueue *virtqueue = &transport->arm_to_pru;

	/* Get an available buffer */
	struct rpmsg_hdr *msg;
	unsigned int msg_len;
	unsigned int head = virtqueue_get_avail_buf(virtqueue, (void**)&msg, &msg_len);

	if (head >= 0xffffff00)
		return RPMSG_NO_BUFFER_AVAILABLE;

	/* Copy the message payload to the local data buffer provided */
	memcpy(data, msg->data, msg->len);
	*src = msg->src;
	*dst = msg->dst;
	*len = msg->len;

	/* Add the used buffer */
	if( virtqueue_add_used_buf(virtqueue, head, msg_len) >= 0xffffff00 ) {
		return RPMSG_INVALID_HEAD;
	}

	/* Kick the ARM host */
	virtqueue_kick(virtqueue);

	return RPMSG_SUCCESS;
}
