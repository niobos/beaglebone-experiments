#pragma once

/* ARM TRM section 17.1.3 */
struct __MAILBOX_REVISION_S {
	volatile unsigned int REV_MINOR   : 6;
	volatile unsigned int REV_CUSTOM  : 2;
	volatile unsigned int REV_MAJOR   : 3;
	volatile unsigned int REV_RTL     : 5;
	volatile unsigned int REV_FUNC    : 12;
	volatile unsigned int reserved    : 2;
	volatile unsigned int REV_SCHEME  : 2;
} __attribute__((packed));

struct __MAILBOX_SYSCONFIG_S {
	volatile unsigned int SOFTRESET  : 1;
	volatile unsigned int reserved0  : 1;
	volatile unsigned int SIDLEMODE  : 2;
	volatile unsigned int reserved1  : 28;
} __attribute__((packed));

struct __MAILBOX_FIFOSTATUS_S {
	volatile unsigned int FIFOFULLMBM      : 1;
	volatile unsigned int MESSAGEVALUEMBM  : 31;
} __attribute__((packed));

struct __MAILBOX_MSGSTATUS_S {
	volatile unsigned int NBOFMSGMBM  : 3;
	volatile unsigned int reserved    : 29;
} __attribute__((packed));

struct __MAILBOX_IRQ_REG_S {
	volatile unsigned int NEW_MSG_STATUS_MB_0   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_0  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_1   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_1  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_2   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_2  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_3   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_3  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_4   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_4  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_5   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_5  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_6   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_6  : 1;
	volatile unsigned int NEW_MSG_STATUS_MB_7   : 1;
	volatile unsigned int NOT_FULL_STATUS_MB_7  : 1;
	volatile unsigned int reserved              : 16;
} __attribute__((packed));

struct __MAILBOX_IRQ_S {
	struct __MAILBOX_IRQ_REG_S STATUS_RAW;
	struct __MAILBOX_IRQ_REG_S STATUS_CLR;
	struct __MAILBOX_IRQ_REG_S ENABLE_SET;
	struct __MAILBOX_IRQ_REG_S ENABLE_CLR;
} __attribute__((packed));

struct __MAILBOX_REGS {
	/* 0x000 */ union {
		volatile unsigned int REVISION;
		struct __MAILBOX_REVISION_S REVISION_bit;
	};
	/* 0x004 */ volatile unsigned int gap004[ (0x010-0x004) / sizeof(unsigned int) ];
	/* 0x010 */ union {
		volatile unsigned int SYSCONFIG;
		struct __MAILBOX_SYSCONFIG_S SYSCONFIG_bit;
	};
	/* 0x014 */ volatile unsigned int gap014[ (0x040-0x014) / sizeof(unsigned int) ];
	/* 0x040 */ volatile unsigned int MESSAGE[8];
	/* 0x060 */ volatile unsigned int gap060[ (0x080-0x060) / sizeof(unsigned int) ];
	/* 0x080 */ union {
		volatile unsigned int FIFOSTATUS[8];
		struct __MAILBOX_FIFOSTATUS_S FIFOSTATUS_bit[8];
	};
	/* 0x0a0 */ volatile unsigned int gap0a0[ (0x0c0-0x0a0) / sizeof(unsigned int) ];
	/* 0x0c0 */ union {
		volatile unsigned int MSGSTATUS[8];
		struct __MAILBOX_MSGSTATUS_S MSGSTATUS_bit[8];
	};
	/* 0x0e0 */ volatile unsigned int gap0e0[ (0x100-0x0e0) / sizeof(unsigned int) ];
	/* 0x100 */ union {
		struct {
			volatile unsigned int IRQSTATUS_RAW_0;
			volatile unsigned int IRQSTATUS_CLR_0;
			volatile unsigned int IRQENABLE_SET_0;
			volatile unsigned int IRQENABLE_CLR_0;
			volatile unsigned int IRQSTATUS_RAW_1;
			volatile unsigned int IRQSTATUS_CLR_1;
			volatile unsigned int IRQENABLE_SET_1;
			volatile unsigned int IRQENABLE_CLR_1;
			volatile unsigned int IRQSTATUS_RAW_2;
			volatile unsigned int IRQSTATUS_CLR_2;
			volatile unsigned int IRQENABLE_SET_2;
			volatile unsigned int IRQENABLE_CLR_2;
			volatile unsigned int IRQSTATUS_RAW_3;
			volatile unsigned int IRQSTATUS_CLR_3;
			volatile unsigned int IRQENABLE_SET_3;
			volatile unsigned int IRQENABLE_CLR_3;
		};
		struct __MAILBOX_IRQ_S IRQ_bit[4];
	};
	/* 0x140 */
};

#define __MAILBOX_REGS_BASE   0x480C8000
#define MAILBOX (*((struct __MAILBOX_REGS *)__MAILBOX_REGS_BASE))

#define MAILBOX_USER_ARM    0
#define MAILBOX_USER_PRU0   1
#define MAILBOX_USER_PRU1   2
#define MAILBOX_USER_WakeM3 3
