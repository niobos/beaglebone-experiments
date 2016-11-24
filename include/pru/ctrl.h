#pragma once

#include "mem_base_addrs.h"

struct __PRU_CTRL_CONTROL_S {
	volatile unsigned int SOFT_RST_N       : 1;
	volatile unsigned int ENABLE           : 1;
	volatile unsigned int SLEEPING         : 1;
	volatile unsigned int COUNTER_ENABLE   : 1;
	volatile unsigned int reserved0        : 4;
	volatile unsigned int SINGLE_STEP      : 1;
	volatile unsigned int reserved1        : 6;
	volatile unsigned int RUNSTATE         : 1;
	volatile unsigned int PCOUNTER_RST_VAL : 16;
} __attribute__((packed));

struct __PRU_CTRL_STATUS_S {
	volatile unsigned int PCOUNTER  : 16;
	volatile unsigned int reserved  : 16;
} __attribute__((packed));

/* PRU reference guide section 5.4 */
struct __PRU_CTRL_REGS {
	/* 0x00 */ union {
		volatile unsigned int CONTROL;
		struct __PRU_CTRL_CONTROL_S CONTROL_bit;
	};
	/* 0x04 */ union {
		volatile unsigned int STATUS;
		struct __PRU_CTRL_STATUS_S STATUS_bit;
	};
	/* 0x08 */ volatile unsigned int WAKEUP_EN;
	/* 0x0c */ volatile unsigned int CYCLECOUNT;
	/* 0x10 */ volatile unsigned int STALLCOUNT;
	/* 0x14 */ volatile unsigned int gap14[3];
	/* 0x20 */ volatile unsigned int CTBIR0;  // TODO: make union
	/* 0x24 */ volatile unsigned int CTBIR1;
	/* 0x28 */ volatile unsigned int CTPPR0;
	/* 0x2c */ volatile unsigned int CTPPR1;
} __attribute__((packed));

#define PRU_0_CTRL (*((struct __PRU_CTRL_REGS *)__PRU_PRU0_CTRL_REGS_BASE))
#define PRU_1_CTRL (*((struct __PRU_CTRL_REGS *)__PRU_PRU1_CTRL_REGS_BASE))

#if defined __AM335X_PRU0__
#define PRU_THIS_CTRL (*((struct __PRU_CTRL_REGS *)__PRU_PRU0_CTRL_REGS_BASE))
#elif defined __AM335X_PRU1__
#define PRU_THIS_CTRL (*((struct __PRU_CTRL_REGS *)__PRU_PRU1_CTRL_REGS_BASE))
#endif
