#pragma once

#include "mem_base_addrs.h"

struct __PRU_CFG_SYSCFR_S {
	volatile unsigned int IDLE_MODE     : 2;
	volatile unsigned int STANDBY_MODE  : 2;
	volatile unsigned int STANDBY_INIT  : 1;
	volatile unsigned int SUB_MWAIT     : 1;
	volatile unsigned int reserved      : 26;
} __attribute__((packed));

struct __PRU_CFG_GPCFG_S {
	volatile unsigned int PRU_GPI_MODE      : 2;
	volatile unsigned int PRU_GPI_CLK_MODE  : 1;
	volatile unsigned int PRU_GPI_DIV0      : 5;
	volatile unsigned int PRU_GPI_DIV1      : 5;
	volatile unsigned int PRU_GPI_SB        : 1;
	volatile unsigned int PRU_GPO_MODE      : 1;
	volatile unsigned int PRU_GPO_DIV0      : 5;
	volatile unsigned int PRU_GPO_DIV1      : 5;
	volatile unsigned int PRU_GPO_SH_SEL    : 1;
	volatile unsigned int reserved          : 6;
} __attribute__((packed));

struct __PRU_CFG_CGR_S {
	volatile unsigned int PRU0_STOP_REQ  : 1;
	volatile unsigned int PRU0_STOP_ACK  : 1;
	volatile unsigned int PRU0_EN        : 1;
	volatile unsigned int PRU1_STOP_REQ  : 1;
	volatile unsigned int PRU1_STOP_ACK  : 1;
	volatile unsigned int PRU1_EN        : 1;
	volatile unsigned int INTC_STOP_REQ  : 1;
	volatile unsigned int INTC_STOP_ACK  : 1;
	volatile unsigned int INTC_EN        : 1;
	volatile unsigned int UART_STOP_REQ  : 1;
	volatile unsigned int UART_STOP_ACK  : 1;
	volatile unsigned int UART_EN        : 1;
	volatile unsigned int ECAP_STOP_REQ  : 1;
	volatile unsigned int ECAP_STOP_ACK  : 1;
	volatile unsigned int ECAP_EN        : 1;
	volatile unsigned int IEP_STOP_REQ   : 1;
	volatile unsigned int IEP_STOP_ACK   : 1;
	volatile unsigned int IEP_EN         : 1;
	volatile unsigned int reserved  : 14;
} __attribute__((packed));

/* PRU reference guide section 10 */
struct __PRU_CFG_REGS {
	/* 0x00 */ volatile unsigned int REVISION;
	/* 0x04 */ union {
		volatile unsigned int SYSCFG;
		struct __PRU_CFG_SYSCFR_S SYSCFG_bit;
	};
	/* 0x08 */ union {
		volatile unsigned int GPCFG0;
		struct __PRU_CFG_GPCFG_S GPCFG0_bit;
	};
	/* 0x0c */ union {
		volatile unsigned int GPCFG1;
		struct __PRU_CFG_GPCFG_S GPCFG1_bit;
	};
	/* 0x10 */ union {
		volatile unsigned int CGR;
		struct __PRU_CFG_CGR_S CGR_bit;
	};
	// TODO
};

#define PRU_CFG (*((struct __PRU_CFG_REGS *)__PRU_CFG_BASE))
