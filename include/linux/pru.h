#pragma once

// Mailbox settings defined by the DeviceTree
// no longer used
// Device tree mailbox@480C8000/mbox_pru0/ti,mbox-tx
#define MAILBOX_PRU0_FROM_ARM_HOST 2
#define MAILBOX_PRU0_TO_ARM_HOST   3
#define MAILBOX_PRU1_FROM_ARM_HOST 4
#define MAILBOX_PRU1_TO_ARM_HOST   5

#if defined __AM335X_PRU0__
#define MAILBOX_PRU_TO_ARM_HOST MAILBOX_PRU0_TO_ARM_HOST
#define MAILBOX_PRU_FROM_ARM_HOST MAILBOX_PRU0_FROM_ARM_HOST
#elif defined __AM335X_PRU1__
#define MAILBOX_PRU_TO_ARM_HOST MAILBOX_PRU1_TO_ARM_HOST
#define MAILBOX_PRU_FROM_ARM_HOST MAILBOX_PRU1_FROM_ARM_HOST
#endif


// interrupt settings defined by the DeviceTree
// Device tree pruss@4a300000/pru0@4a334000/interrupts
#define INTERRUPT_PRU0_VRING 16 // PRU to ARM
#define INTERRUPT_PRU0_KICK  17 // ARM to PRU
#define INTERRUPT_PRU1_VRING 18 // PRU to ARM
#define INTERRUPT_PRU1_KICK  19 // ARM to PRU

#if defined __AM335X_PRU0__
#define INTERRUPT_PRU_VRING INTERRUPT_PRU0_VRING
#define INTERRUPT_PRU_KICK INTERRUPT_PRU0_KICK
#elif defined __AM335X_PRU1__
#define INTERRUPT_PRU_VRING INTERRUPT_PRU1_VRING
#define INTERRUPT_PRU_KICK INTERRUPT_PRU1_KICK
#endif
