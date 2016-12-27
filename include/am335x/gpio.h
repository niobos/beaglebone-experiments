#pragma once

struct __GPIO_REVISION_S {
	unsigned int minor    : 6;
	unsigned int custom   : 2;
	unsigned int major    : 3;
	unsigned int rtl      : 5;
	unsigned int func     : 12;
	unsigned int reserved : 2;
	unsigned int scheme   : 2;
} __attribute__((packed));

struct __GPIO_SYSCONFIG_S {
	unsigned int autoidle  : 1;
	unsigned int softreset : 1;
	unsigned int enawakup  : 1;
	unsigned int idlemode  : 2;
	unsigned int reserved  : 27;
} __attribute__((packed));

struct __GPIO_CTRL_S {
	unsigned int disablemodule : 1;
	unsigned int gatingratio   : 2;
	unsigned int reserved      : 29;
} __attribute__((packed));

struct __GPIO_DEBOUNCINGTIME_S {
	unsigned int debountetime : 8;
	unsigned int reserved     : 24;
} __attribute__((packed));

/* ARM TRM section 25.4 */
struct __GPIO_REGS {
	/* 0x000 */ union {
		volatile unsigned int REVISION;
		struct __GPIO_REVISION_S REVISION_bit;
	};
	/* 0x004 */ volatile unsigned int gap004[3];
	/* 0x010 */ union {
		volatile unsigned int SYSCONFIG;
		struct __GPIO_SYSCONFIG_S SYSCONFIG_bit;
	};
	/* 0x014 */ volatile unsigned int gap014[3];
	/* 0x020 */ volatile unsigned int EOI;
	/* 0x024 */ volatile unsigned int IRQSTATUS_RAW_0;
	/* 0x028 */ volatile unsigned int IRQSTATUS_RAW_1;
	/* 0x02C */ volatile unsigned int IRQSTATUS_0;
	/* 0x030 */ volatile unsigned int IRQSTATUS_1;
	/* 0x034 */ volatile unsigned int IRQSTATUS_SET_0;
	/* 0x038 */ volatile unsigned int IRQSTATUS_SET_1;
	/* 0x03C */ volatile unsigned int IRQSTATUS_CLR_0;
	/* 0x040 */ volatile unsigned int IRQSTATUS_CLR_1;
	/* 0x044 */ volatile unsigned int IRQWAKEN_0;
	/* 0x048 */ volatile unsigned int IRQWAKEN_1;
	/* 0x04C */ volatile unsigned int gap04C[50];
	/* 0x114 */ volatile unsigned int SYSSTATUS;
	/* 0x118 */ volatile unsigned int gap118[6];
	/* 0x130 */ union {
		volatile unsigned int CTRL;
		struct __GPIO_CTRL_S CTRL_bit;
	};
	/* 0x134 */ volatile unsigned int OE;
	/* 0x138 */ volatile unsigned int DATAIN;
	/* 0x13C */ volatile unsigned int DATAOUT;
	/* 0x140 */ volatile unsigned int LEVELDETECT0;
	/* 0x144 */ volatile unsigned int LEVELDETECT1;
	/* 0x148 */ volatile unsigned int RISINGDETECT;
	/* 0x14C */ volatile unsigned int FALLINGDETECT;
	/* 0x150 */ volatile unsigned int DEBOUNCENABLE;
	/* 0x154 */ union {
		volatile unsigned int DEBOUNCINGTIME;
		struct __GPIO_DEBOUNCINGTIME_S DEBOUNCINGTIME_bit;
	};
	/* 0x158 */ volatile unsigned int gap158[14];
	/* 0x190 */ volatile unsigned int CLEARDATAOUT;
	/* 0x194 */ volatile unsigned int SETDATAOUT;
};

#define __GPIO0_REGS_BASE   0x44E07000
#define __GPIO1_REGS_BASE   0x4804C000
#define __GPIO2_REGS_BASE   0x481AC000
#define __GPIO3_REGS_BASE   0x481AE000

#define GPIO0 (*((struct __GPIO_REGS *)__GPIO0_REGS_BASE))
#define GPIO1 (*((struct __GPIO_REGS *)__GPIO1_REGS_BASE))
#define GPIO2 (*((struct __GPIO_REGS *)__GPIO2_REGS_BASE))
#define GPIO3 (*((struct __GPIO_REGS *)__GPIO3_REGS_BASE))
