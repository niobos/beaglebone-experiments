/*
 *  ======== rsc_table_am335x_pru.h ========
 *
 *  Define the resource table entries for all PRU cores. This will be
 *  incorporated into corresponding base images, and used by the remoteproc
 *  on the host-side to allocated/reserve resources.
 *
 */

#ifndef _RSC_TABLE_PRU_H_
#define _RSC_TABLE_PRU_H_

#include <stdint.h>

struct my_resource_table {
	struct resource_table base;

	uint32_t offset[1]; /* Should match 'num' in actual definition */
};

#if !defined(__GNUC__)
  #pragma DATA_SECTION(am335x_pru_remoteproc_ResourceTable, ".resource_table")
  #pragma RETAIN(am335x_pru_remoteproc_ResourceTable)
  #define __resource_table	/* */
#else
  #define __resource_table __attribute__((section(".resource_table")))
#endif
struct my_resource_table am335x_pru_remoteproc_ResourceTable __resource_table = {
	{
		1,		/* we're the first version that implements this */
		0,		/* number of entries in the table */
		{ 0, 0 },	/* reserved, must be zero */
	},
	{ 0 },
};

#endif /* _RSC_TABLE_PRU_H_ */
