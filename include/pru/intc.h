#pragma once

#include "mem_base_addrs.h"

/* PRU reference guide section 6.4 */
struct __PRU_INTC_REVID_S {
	volatile unsigned int REV_MINOR   : 6;
	volatile unsigned int REV_CUSTOM  : 2;
	volatile unsigned int REV_MAJOR   : 3;
	volatile unsigned int REV_RTL     : 5;
	volatile unsigned int REV_MODULE  : 12;
	volatile unsigned int reserved    : 2;
	volatile unsigned int REV_SCHEME  : 2;
} __attribute__((packed));

struct __PRU_INTC_CR_S {
	volatile unsigned int reserved0  : 2;
	volatile unsigned int NEST_MODE  : 2;
	volatile unsigned int reserved1  : 28;
} __attribute__((packed));

struct __PRU_INTC_GER_S {
	volatile unsigned int ENABLE_HINT_ANY  : 1;
	volatile unsigned int reserved         : 31;
} __attribute__((packed));

struct __PRU_INTC_GNLR_S {
	volatile unsigned int GLB_NEST_LEVEL  : 9;
	volatile unsigned int reserved        : 22;
	volatile unsigned int AUTO_OVERRIDE   : 1;
} __attribute__((packed));

struct __PRU_INTC_INDEX_S {
	volatile unsigned int INDEX     : 10;
	volatile unsigned int reserved  : 22;
} __attribute__((packed));

struct __PRU_INTC_GPIR_S {
	volatile unsigned int GLB_PRI_INTR  : 10;
	volatile unsigned int reserved      : 21;
	volatile unsigned int GLB_NONE      : 1;
} __attribute__((packed));

struct __PRU_INTC_INT_S {
	volatile unsigned int INT0   : 1;
	volatile unsigned int INT1   : 1;
	volatile unsigned int INT2   : 1;
	volatile unsigned int INT3   : 1;
	volatile unsigned int INT4   : 1;
	volatile unsigned int INT5   : 1;
	volatile unsigned int INT6   : 1;
	volatile unsigned int INT7   : 1;
	volatile unsigned int INT8   : 1;
	volatile unsigned int INT9   : 1;
	volatile unsigned int INT10  : 1;
	volatile unsigned int INT11  : 1;
	volatile unsigned int INT12  : 1;
	volatile unsigned int INT13  : 1;
	volatile unsigned int INT14  : 1;
	volatile unsigned int INT15  : 1;
	volatile unsigned int INT16  : 1;
	volatile unsigned int INT17  : 1;
	volatile unsigned int INT18  : 1;
	volatile unsigned int INT19  : 1;
	volatile unsigned int INT20  : 1;
	volatile unsigned int INT21  : 1;
	volatile unsigned int INT22  : 1;
	volatile unsigned int INT23  : 1;
	volatile unsigned int INT24  : 1;
	volatile unsigned int INT25  : 1;
	volatile unsigned int INT26  : 1;
	volatile unsigned int INT27  : 1;
	volatile unsigned int INT28  : 1;
	volatile unsigned int INT29  : 1;
	volatile unsigned int INT30  : 1;
	volatile unsigned int INT31  : 1;
	volatile unsigned int INT32  : 1;
	volatile unsigned int INT33  : 1;
	volatile unsigned int INT34  : 1;
	volatile unsigned int INT35  : 1;
	volatile unsigned int INT36  : 1;
	volatile unsigned int INT37  : 1;
	volatile unsigned int INT38  : 1;
	volatile unsigned int INT39  : 1;
	volatile unsigned int INT40  : 1;
	volatile unsigned int INT41  : 1;
	volatile unsigned int INT42  : 1;
	volatile unsigned int INT43  : 1;
	volatile unsigned int INT44  : 1;
	volatile unsigned int INT45  : 1;
	volatile unsigned int INT46  : 1;
	volatile unsigned int INT47  : 1;
	volatile unsigned int INT48  : 1;
	volatile unsigned int INT49  : 1;
	volatile unsigned int INT50  : 1;
	volatile unsigned int INT51  : 1;
	volatile unsigned int INT52  : 1;
	volatile unsigned int INT53  : 1;
	volatile unsigned int INT54  : 1;
	volatile unsigned int INT55  : 1;
	volatile unsigned int INT56  : 1;
	volatile unsigned int INT57  : 1;
	volatile unsigned int INT58  : 1;
	volatile unsigned int INT59  : 1;
	volatile unsigned int INT60  : 1;
	volatile unsigned int INT61  : 1;
	volatile unsigned int INT62  : 1;
	volatile unsigned int INT63  : 1;
} __attribute__((packed));

struct __PRU_INTC_CHMAP_S {
	volatile unsigned int CH_MAP_0     : 4;
	volatile unsigned int reserved_0   : 4;
	volatile unsigned int CH_MAP_1     : 4;
	volatile unsigned int reserved_1   : 4;
	volatile unsigned int CH_MAP_2     : 4;
	volatile unsigned int reserved_2   : 4;
	volatile unsigned int CH_MAP_3     : 4;
	volatile unsigned int reserved_3   : 4;
	volatile unsigned int CH_MAP_4     : 4;
	volatile unsigned int reserved_4   : 4;
	volatile unsigned int CH_MAP_5     : 4;
	volatile unsigned int reserved_5   : 4;
	volatile unsigned int CH_MAP_6     : 4;
	volatile unsigned int reserved_6   : 4;
	volatile unsigned int CH_MAP_7     : 4;
	volatile unsigned int reserved_7   : 4;
	volatile unsigned int CH_MAP_8     : 4;
	volatile unsigned int reserved_8   : 4;
	volatile unsigned int CH_MAP_9     : 4;
	volatile unsigned int reserved_9   : 4;
	volatile unsigned int CH_MAP_10    : 4;
	volatile unsigned int reserved_10  : 4;
	volatile unsigned int CH_MAP_11    : 4;
	volatile unsigned int reserved_11  : 4;
	volatile unsigned int CH_MAP_12    : 4;
	volatile unsigned int reserved_12  : 4;
	volatile unsigned int CH_MAP_13    : 4;
	volatile unsigned int reserved_13  : 4;
	volatile unsigned int CH_MAP_14    : 4;
	volatile unsigned int reserved_14  : 4;
	volatile unsigned int CH_MAP_15    : 4;
	volatile unsigned int reserved_15  : 4;
	volatile unsigned int CH_MAP_16    : 4;
	volatile unsigned int reserved_16  : 4;
	volatile unsigned int CH_MAP_17    : 4;
	volatile unsigned int reserved_17  : 4;
	volatile unsigned int CH_MAP_18    : 4;
	volatile unsigned int reserved_18  : 4;
	volatile unsigned int CH_MAP_19    : 4;
	volatile unsigned int reserved_19  : 4;
	volatile unsigned int CH_MAP_20    : 4;
	volatile unsigned int reserved_20  : 4;
	volatile unsigned int CH_MAP_21    : 4;
	volatile unsigned int reserved_21  : 4;
	volatile unsigned int CH_MAP_22    : 4;
	volatile unsigned int reserved_22  : 4;
	volatile unsigned int CH_MAP_23    : 4;
	volatile unsigned int reserved_23  : 4;
	volatile unsigned int CH_MAP_24    : 4;
	volatile unsigned int reserved_24  : 4;
	volatile unsigned int CH_MAP_25    : 4;
	volatile unsigned int reserved_25  : 4;
	volatile unsigned int CH_MAP_26    : 4;
	volatile unsigned int reserved_26  : 4;
	volatile unsigned int CH_MAP_27    : 4;
	volatile unsigned int reserved_27  : 4;
	volatile unsigned int CH_MAP_28    : 4;
	volatile unsigned int reserved_28  : 4;
	volatile unsigned int CH_MAP_29    : 4;
	volatile unsigned int reserved_29  : 4;
	volatile unsigned int CH_MAP_30    : 4;
	volatile unsigned int reserved_30  : 4;
	volatile unsigned int CH_MAP_31    : 4;
	volatile unsigned int reserved_31  : 4;
	volatile unsigned int CH_MAP_32    : 4;
	volatile unsigned int reserved_32  : 4;
	volatile unsigned int CH_MAP_33    : 4;
	volatile unsigned int reserved_33  : 4;
	volatile unsigned int CH_MAP_34    : 4;
	volatile unsigned int reserved_34  : 4;
	volatile unsigned int CH_MAP_35    : 4;
	volatile unsigned int reserved_35  : 4;
	volatile unsigned int CH_MAP_36    : 4;
	volatile unsigned int reserved_36  : 4;
	volatile unsigned int CH_MAP_37    : 4;
	volatile unsigned int reserved_37  : 4;
	volatile unsigned int CH_MAP_38    : 4;
	volatile unsigned int reserved_38  : 4;
	volatile unsigned int CH_MAP_39    : 4;
	volatile unsigned int reserved_39  : 4;
	volatile unsigned int CH_MAP_40    : 4;
	volatile unsigned int reserved_40  : 4;
	volatile unsigned int CH_MAP_41    : 4;
	volatile unsigned int reserved_41  : 4;
	volatile unsigned int CH_MAP_42    : 4;
	volatile unsigned int reserved_42  : 4;
	volatile unsigned int CH_MAP_43    : 4;
	volatile unsigned int reserved_43  : 4;
	volatile unsigned int CH_MAP_44    : 4;
	volatile unsigned int reserved_44  : 4;
	volatile unsigned int CH_MAP_45    : 4;
	volatile unsigned int reserved_45  : 4;
	volatile unsigned int CH_MAP_46    : 4;
	volatile unsigned int reserved_46  : 4;
	volatile unsigned int CH_MAP_47    : 4;
	volatile unsigned int reserved_47  : 4;
	volatile unsigned int CH_MAP_48    : 4;
	volatile unsigned int reserved_48  : 4;
	volatile unsigned int CH_MAP_49    : 4;
	volatile unsigned int reserved_49  : 4;
	volatile unsigned int CH_MAP_50    : 4;
	volatile unsigned int reserved_50  : 4;
	volatile unsigned int CH_MAP_51    : 4;
	volatile unsigned int reserved_51  : 4;
	volatile unsigned int CH_MAP_52    : 4;
	volatile unsigned int reserved_52  : 4;
	volatile unsigned int CH_MAP_53    : 4;
	volatile unsigned int reserved_53  : 4;
	volatile unsigned int CH_MAP_54    : 4;
	volatile unsigned int reserved_54  : 4;
	volatile unsigned int CH_MAP_55    : 4;
	volatile unsigned int reserved_55  : 4;
	volatile unsigned int CH_MAP_56    : 4;
	volatile unsigned int reserved_56  : 4;
	volatile unsigned int CH_MAP_57    : 4;
	volatile unsigned int reserved_57  : 4;
	volatile unsigned int CH_MAP_58    : 4;
	volatile unsigned int reserved_58  : 4;
	volatile unsigned int CH_MAP_59    : 4;
	volatile unsigned int reserved_59  : 4;
	volatile unsigned int CH_MAP_60    : 4;
	volatile unsigned int reserved_60  : 4;
	volatile unsigned int CH_MAP_61    : 4;
	volatile unsigned int reserved_61  : 4;
	volatile unsigned int CH_MAP_62    : 4;
	volatile unsigned int reserved_62  : 4;
	volatile unsigned int CH_MAP_63    : 4;
	volatile unsigned int reserved_63  : 4;
} __attribute__((packed));

struct __PRU_INTC_HMAP_S {
	volatile unsigned int HINT_MAP_0   : 4;
	volatile unsigned int reserved_0   : 4;
	volatile unsigned int HINT_MAP_1   : 4;
	volatile unsigned int reserved_1   : 4;
	volatile unsigned int HINT_MAP_2   : 4;
	volatile unsigned int reserved_2   : 4;
	volatile unsigned int HINT_MAP_3   : 4;
	volatile unsigned int reserved_3   : 4;
	volatile unsigned int HINT_MAP_4   : 4;
	volatile unsigned int reserved_4   : 4;
	volatile unsigned int HINT_MAP_5   : 4;
	volatile unsigned int reserved_5   : 4;
	volatile unsigned int HINT_MAP_6   : 4;
	volatile unsigned int reserved_6   : 4;
	volatile unsigned int HINT_MAP_7   : 4;
	volatile unsigned int reserved_7   : 4;
	volatile unsigned int HINT_MAP_8   : 4;
	volatile unsigned int reserved_8   : 4;
	volatile unsigned int HINT_MAP_9   : 4;
	volatile unsigned int reserved_9   : 20;
} __attribute__((packed));

struct __PRU_INTC_HIPIR_S {
	volatile unsigned int PRI_HINT_0   : 10;
	volatile unsigned int reserved     : 21;
	volatile unsigned int NONE_HINT_0  : 1;
} __attribute__((packed));

struct __PRU_INTC_HINLR_S {
	volatile unsigned int NEST_HINT      : 9;
	volatile unsigned int reserved       : 22;
	volatile unsigned int AUTO_OVERRIDE  : 1;
} __attribute__((packed));

struct __PRU_INTC_HIER_S {
	volatile unsigned int ENABLE_HINT_0  : 1;
	volatile unsigned int ENABLE_HINT_1  : 1;
	volatile unsigned int ENABLE_HINT_2  : 1;
	volatile unsigned int ENABLE_HINT_3  : 1;
	volatile unsigned int ENABLE_HINT_4  : 1;
	volatile unsigned int ENABLE_HINT_5  : 1;
	volatile unsigned int ENABLE_HINT_6  : 1;
	volatile unsigned int ENABLE_HINT_7  : 1;
	volatile unsigned int ENABLE_HINT_8  : 1;
	volatile unsigned int ENABLE_HINT_9  : 1;
	volatile unsigned int reserved       : 22;
} __attribute__((packed));

struct __PRU_INTC_REGS {
	/* 0x0000 */ union {
		volatile unsigned int REVID;
		struct __PRU_INTC_REVID_S REVID_bit;
	};
	/* 0x0004 */ union {
		volatile unsigned int CR;
		struct __PRU_INTC_CR_S CR_bit;
	};
	/* 0x0008 */ volatile unsigned int gap0008[2];
	/* 0x0010 */ union {
		volatile unsigned int GER;
		struct __PRU_INTC_GER_S GER_bit;
	};
	/* 0x0014 */ volatile unsigned int gap0014[2];
	/* 0x001c */ union {
		volatile unsigned int GNLR;
		struct __PRU_INTC_GNLR_S GNLR_bit;
	};
	/* 0x0020 */ union {
		volatile unsigned int SISR;
		struct __PRU_INTC_INDEX_S SISR_bit;
	};
	/* 0x0024 */ union {
		volatile unsigned int SICR;
		struct __PRU_INTC_INDEX_S SICR_bit;
	};
	/* 0x0028 */ union {
		volatile unsigned int EISR;
		struct __PRU_INTC_INDEX_S EISR_bit;
	};
	/* 0x002c */ union {
		volatile unsigned int EICR;
		struct __PRU_INTC_INDEX_S EICR_bit;
	};
	/* 0x0030 */ volatile unsigned int gap0030;
	/* 0x0034 */ union {
		volatile unsigned int HIEISR;
		struct __PRU_INTC_INDEX_S HIEISR_bit;
	};
	/* 0x0038 */ union {
		volatile unsigned int HIDISR;
		struct __PRU_INTC_INDEX_S HIDISR_bit;
	};
	/* 0x003c */ volatile unsigned int gap003c[17];
	/* 0x0080 */ union {
		volatile unsigned int GPIR;
		struct __PRU_INTC_GPIR_S GPIR_bit;
	};
	/* 0x0084 */ volatile unsigned int gap0084[95];
	/* 0x0200 */ union {
		struct {
			volatile unsigned int SRSR0;
			volatile unsigned int SRSR1;
		};
		struct __PRU_INTC_INT_S SRSR_bit;
	};
	/* 0x0208 */ volatile unsigned int gap0208[30];
	/* 0x0280 */ union {
		struct {
			volatile unsigned int SECR0;
			volatile unsigned int SECR1;
		};
		struct __PRU_INTC_INT_S SECR_bit;
	};
	/* 0x0288 */ volatile unsigned int gap0288[30];
	/* 0x0300 */ union {
		struct {
			volatile unsigned int ESR0;
			volatile unsigned int ESR1;
		};
		struct __PRU_INTC_INT_S ESR_bit;
	};
	/* 0x0308 */ volatile unsigned int gap0308[30];
	/* 0x0380 */ union {
		struct {
			volatile unsigned int ECR0;
			volatile unsigned int ECR1;
		};
		struct __PRU_INTC_INT_S ECR_bit;
	};
	/* 0x0388 */ volatile unsigned int gap0388[30];
	/* 0x0400 */ union {
		struct {
			volatile unsigned int CMR0;
			volatile unsigned int CMR1;
			volatile unsigned int CMR2;
			volatile unsigned int CMR3;
			volatile unsigned int CMR4;
			volatile unsigned int CMR5;
			volatile unsigned int CMR6;
			volatile unsigned int CMR7;
			volatile unsigned int CMR8;
			volatile unsigned int CMR9;
			volatile unsigned int CMR10;
			volatile unsigned int CMR11;
			volatile unsigned int CMR12;
			volatile unsigned int CMR13;
			volatile unsigned int CMR14;
			volatile unsigned int CMR15;
		};
		struct __PRU_INTC_CHMAP_S CMR_bit;
	};
	/* 0x0440 */ volatile unsigned int gap0440[240];
	/* 0x0800 */ union {
		struct {
			volatile unsigned int HMR0;
			volatile unsigned int HMR1;
			volatile unsigned int HMR2;
		};
		struct __PRU_INTC_HMAP_S HMR_bit;
	};
	/* 0x080c */ volatile unsigned int gap080c[61];
	/* 0x0900 */ union {
		struct {
			volatile unsigned int HIPIR0;
			volatile unsigned int HIPIR1;
			volatile unsigned int HIPIR2;
			volatile unsigned int HIPIR3;
			volatile unsigned int HIPIR4;
			volatile unsigned int HIPIR5;
			volatile unsigned int HIPIR6;
			volatile unsigned int HIPIR7;
			volatile unsigned int HIPIR8;
			volatile unsigned int HIPIR9;
		};
		struct __PRU_INTC_HIPIR_S HIPIR_bit[10];
	};
	/* 0x0928 */ volatile unsigned int gap0928[246];
	/* 0x0d00 */ union {
		struct {
			volatile unsigned int SIPR0;
			volatile unsigned int SIPR1;
		};
		struct __PRU_INTC_INT_S SIPR_bit;
	};
	/* 0x0d08 */ volatile unsigned int gap0d08[30];
	/* 0x0d80 */ union {
		struct {
			volatile unsigned int SITR0;
			volatile unsigned int SITR1;
		};
		struct __PRU_INTC_INT_S SITR_bit;
	};
	/* 0x0d88 */ volatile unsigned int gap0d88[222];
	/* 0x1100 */ union {
		struct {
			volatile unsigned int HINLR0;
			volatile unsigned int HINLR1;
			volatile unsigned int HINLR2;
			volatile unsigned int HINLR3;
			volatile unsigned int HINLR4;
			volatile unsigned int HINLR5;
			volatile unsigned int HINLR6;
			volatile unsigned int HINLR7;
			volatile unsigned int HINLR8;
			volatile unsigned int HINLR9;
		};
		struct __PRU_INTC_HINLR_S HINLR_bit[10];
	};
	/* 0x1128 */ volatile unsigned int gap1128[246];
	/* 0x1500 */ union {
		volatile unsigned int HIER;
		struct __PRU_INTC_HIER_S HIER_bit;
	};
};

#define PRU_INTC (*((struct __PRU_INTC_REGS *)__PRU_INTC_BASE))

#define PRU_INT_HOST0 (1<<30)
#define PRU_INT_HOST1 (1<<31)
