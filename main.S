#define CONST_PRUCFG 4

#define GPIO1 0x4804c000
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194

	.text
	.section .init0, "x"
	.global	__start

__start:
	// Enable OCP master port to access external memory
	LBCO      r0, CONST_PRUCFG, 4, 4
	CLR       r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO      r0, CONST_PRUCFG, 4, 4

main:
	LDI32 r2, 1<<21
	LDI32 r3, GPIO1 | GPIO_SETDATAOUT
	SBBO r2, r3, 0, 4

	LDI32 r0, 0x08000000
DEL1:
	SUB r0, r0, 1
	QBNE DEL1, r0, 0

	LDI32 r2, 1<<21
	LDI32 r3, GPIO1 | GPIO_CLEARDATAOUT
	SBBO r2, r3, 0, 4

	LDI32 r0, 0x08000000
DEL2:
	SUB r0, r0, 1
	QBNE DEL2, r0, 0

	JMP main
	// unreachable
	ldi r31.b0, 35 // interrupt to host
	halt


	/* Dummy data, required by remoteproc loader */
	.data
	.section .resource_table,"aw",@progbits
my_resource_table:
	.word	1, 0, 0, 0	/* struct resource_table base */
	.word	0		/* uint32_t offset[1] */
