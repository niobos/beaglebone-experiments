#pragma once

#include <stdint.h>

// from Linux/include/linux/rpmsg.h
enum rpmsg_ns_flags {
	RPMSG_NS_CREATE  = 0,
	RPMSG_NS_DESTROY = 1,
};

#define VIRTIO_RPMSG_F_NS 0 /* RP supports name service notifications */

/* The maximum size of the channel name and description */
#define RPMSG_NAME_SIZE 32
/* The maximum size of the buffer (including the header) */
#define RPMSG_BUF_SIZE  512

struct rpmsg_hdr {
	uint32_t  src;
	uint32_t  dst;
	uint32_t  reserved;
	uint16_t  len;
	uint16_t  flags;
	uint8_t   data[0];
} __attribute__((packed));

struct rpmsg_ns_msg {
	char      name[RPMSG_NAME_SIZE];
	char      desc[RPMSG_NAME_SIZE];
	uint32_t  addr;
	uint32_t  flags;
} __attribute__((packed));
