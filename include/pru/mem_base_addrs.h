#pragma once

/* Local memory map (PRU section 3.1.20) */
#define __PRU_LOCAL_DATA_RAM_BASE   0x00000000
#define __PRU_PEER_DATA_RAM_BASE    0x00002000
#define __PRU_SHARED_DATA_RAM_BASE  0x00010000
#define __PRU_INTC_BASE             0x00020000

#define __PRU_PRU0_CTRL_REGS_BASE   0x00022000
#define __PRU_PRU1_CTRL_REGS_BASE   0x00024000
#if defined __AM335X_PRU0__
#define __PRU_PRU_CTRL_REGS_BASE __PRU_PRU0_CTRL_REGS_BASE
#elif defined __AM335X_PRU1__
#define __PRU_PRU_CTRL_REGS_BASE __PRU_PRU1_CTRL_REGS_BASE
#endif

#define __PRU_CFG_BASE              0x00026000
#define __PRU_UART0_BASE            0x00028000
#define __PRU_IEP_BASE              0x0002E000
#define __PRU_ECAP0_BASE            0x00030000
#define __PRU_MII_RT_CFG_BASE       0x00032000
#define __PRU_MII_MDIO_BASE         0x00034000
#define __PRU_SYS_OCP_HP0_BASE      0x00080000
