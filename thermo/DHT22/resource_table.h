#pragma once

#include <linux/rsc_types_pru.h>
#include <linux/virtio_ids.h>
#include <linux/rpmsg.h>
#include <linux/pru.h>

#include <stddef.h>  // for offsetof()

struct my_resource_table {
	struct resource_table base;

	uint32_t offset[2]; /* Should match 'num' in actual definition */

	/* intc definition */
	struct fw_rsc_hdr pru_ints_hdr;
		struct fw_rsc_custom pru_ints_custom;
		struct fw_rsc_custom_ints pru_ints;

	/* rpmsg vdev entry */
	struct fw_rsc_hdr rpmsg_vdev_hdr;
		struct fw_rsc_vdev rpmsg_vdev;
		struct fw_rsc_vdev_vring rpmsg_vring_pru_to_arm;
		struct fw_rsc_vdev_vring rpmsg_vring_arm_to_pru;
};

struct ch_map pru_intc_map[] = {
	// { <system interrupt>, <channel> }
	{INTERRUPT_PRU_VRING, 2},
	{INTERRUPT_PRU_KICK,  0},
};

#if !defined(__GNUC__)
  #pragma DATA_SECTION(am335x_pru_remoteproc_ResourceTable, ".resource_table")
  #pragma RETAIN(am335x_pru_remoteproc_ResourceTable)
  #define __resource_table	/* */
#else
  #define __resource_table __attribute__((section(".resource_table")))
#endif
struct my_resource_table rproc_resource_table __resource_table = {
	{
		/* version           */ 1,
		/* number of entries */ 2,
		/* reserved          */ { 0, 0 },
	},
	{
		offsetof(struct my_resource_table, pru_ints_hdr),
		offsetof(struct my_resource_table, rpmsg_vdev_hdr),
	},

	{ /* pru_ints_hdr */
		/* .type */ RSC_CUSTOM,
		}, {/* pru_ints_custom */
			/* .sub_type */ TYPE_PRU_INTS,
			/* .size     */ sizeof(struct fw_rsc_custom_ints),
		}, {/* struct fw_rsc_custom_ints */
			/* .version = */ 0x0000,

			/* Channel-to-host mapping, 255 for unused
			 * Mapping Channel-0 to Host-0 (PRU0 R31 bit 31)
			 */
			/* .channel_host */ {
				  0, 255,   2, 255, 255,
				255, 255, 255, 255, 255
			},

			/* .num_evts */ (sizeof(pru_intc_map) / sizeof(struct ch_map)),
			/* .evts     */ pru_intc_map,
	},

	{ /* rpmsg_vdev_hdr */
		/* .type */ RSC_VDEV,
		}, { /* rpmsg_vdev */
			/* .id            */ VIRTIO_ID_RPMSG,
			/* .notifyid      */ 0,
			/* .dfeatures     */ (1 << VIRTIO_RPMSG_F_NS),
			/* .gfeatures     */ 0,  /* filled in by ARM on load */
			/* .config_len    */ 0,
			/* .status        */ 0,  /* filled in by ARM */
			/* .num_of_vrings */ 2,
			/* .reserved      */ { 0, 0 },
		},
		{
			/* .da       */ 0,  /* filled in by ARM on load */
			/* .align    */ 16,
			/* .num      */ 16,
			/* .notifyid */ 0,
			/* .reserved */ 0,
		},
		{
			/* .da       */ 0,  /* filled in by ARM on load */
			/* .align    */ 16,
			/* .num      */ 16,
			/* .notifyid */ 0,
			/* .reserved */ 0,
	},
};
