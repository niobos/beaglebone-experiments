#pragma once

/* ARM TRM section 25.4 */
struct __GPIO_REGS {
	/* 0x000 */ volatile unsigned int REVISION;
	/* 0x004 */ volatile unsigned int gap004[3];
	/* 0x010 */ union {
		volatile unsigned int SYSCONFIG;
		//struct __GPIO_SYSCONFIG SYSCONFIG_bit;
	};
	/* 0x014 */ volatile unsigned int gap014[3];
	/* 0x020 */ union {
		volatile unsigned int EOI;
		//struct __GPIO_EOI EOI_bit;
	};
	/* 0x024 */ union {
		volatile unsigned int IRQSTATUS_RAW_0;
		//struct __GPIO_IRQSTATUS_RAW_0 IRQSTATUS_RAW_0_bit;
	};
	/* 0x028 */ union {
		volatile unsigned int IRQSTATUS_RAW_1;
		//struct __GPIO_IRQSTATUS_RAW_1 IRQSTATUS_RAW_1_bit;
	};
	/* 0x02C */ union {
		volatile unsigned int IRQSTATUS_0;
		//struct __GPIO_IRQSTATUS_0 IRQSTATUS_0_bit;
	};
	/* 0x030 */ union {
		volatile unsigned int IRQSTATUS_1;
		//struct __GPIO_IRQSTATUS_1 IRQSTATUS_1_bit;
	};
	/* 0x034 */ union {
		volatile unsigned int IRQSTATUS_SET_0;
		//struct __GPIO_IRQSTATUS_SET_0 IRQSTATUS_SET_0_bit;
	};
	/* 0x038 */ union {
		volatile unsigned int IRQSTATUS_SET_1;
		//struct __GPIO_IRQSTATUS_SET_1 IRQSTATUS_SET_1_bit;
	};
	/* 0x03C */ union {
		volatile unsigned int IRQSTATUS_CLR_0;
		//struct __GPIO_IRQSTATUS_CLR_0 IRQSTATUS_CLR_0_bit;
	};
	/* 0x040 */ union {
		volatile unsigned int IRQSTATUS_CLR_1;
		//struct __GPIO_IRQSTATUS_CLR_1 IRQSTATUS_CLR_1_bit;
	};
	/* 0x044 */ union {
		volatile unsigned int IRQWAKEN_0;
		//struct __GPIO_IRQWAKEN_0 IRQWAKEN_0_bit;
	};
	/* 0x048 */ union {
		volatile unsigned int IRQWAKEN_1;
		//struct __GPIO_IRQWAKEN_1 IRQWAKEN_1_bit;
	};
	/* 0x04C */ volatile unsigned int gap04C[50];
	/* 0x114 */ union {
		volatile unsigned int SYSSTATUS;
		//struct __GPIO_SYSSTATUS SYSSTATUS_bit;
	};
	/* 0x118 */ volatile unsigned int gap118[6];
	/* 0x130 */ union {
		volatile unsigned int CTRL;
		//struct __GPIO_CTRL CTRL_bit;
	};
	/* 0x134 */ union {
		volatile unsigned int OE;
		//struct __GPIO_OE OE_bit;
	};
	/* 0x138 */ union {
		volatile unsigned int DATAIN;
		//struct __GPIO_DATAIN DATAIN_bit;
	};
	/* 0x13C */ union {
		volatile unsigned int DATAOUT;
		//struct __GPIO_DATAOUT DATAOUT_bit;
	};
	/* 0x140 */ union {
		volatile unsigned int LEVELDETECT0;
		//struct __GPIO_LEVELDETECT0 LEVELDETECT0_bit;
	};
	/* 0x144 */ union {
		volatile unsigned int LEVELDETECT1;
		//struct __GPIO_LEVELDETECT1 LEVELDETECT1_bit;
	};
	/* 0x148 */ union {
		volatile unsigned int RISINGDETECT;
		//struct __GPIO_RISINGDETECT RISINGDETECT_bit;
	};
	/* 0x14C */ union {
		volatile unsigned int FALLINGDETECT;
		//struct __GPIO_FALLINGDETECT FALLINGDETECT_bit;
	};
	/* 0x150 */ union {
		volatile unsigned int DEBOUNCENABLE;
		//struct __GPIO_DEBOUNCENABLE DEBOUNCENABLE_bit;
	};
	/* 0x154 */ union {
		volatile unsigned int DEBOUNCINGTIME;
		//struct __GPIO_DEBOUNCINGTIME DEBOUNCINGTIME_bit;
	};
	/* 0x158 */ volatile unsigned int gap158[14];
	/* 0x190 */ union {
		volatile unsigned int CLEARDATAOUT;
		//struct __GPIO_CLEARDATAOUT CLEARDATAOUT_bit;
	};
	/* 0x194 */ union {
		volatile unsigned int SETDATAOUT;
		//struct __GPIO_SETDATAOUT SETDATAOUT_bit;
	};
};

#define __GPIO1_REGS_BASE   0x4804C000

#define GPIO1 (*((struct __GPIO_REGS *)__GPIO1_REGS_BASE))
