#pragma once

static inline void write_r30(unsigned int val) {
	asm volatile (
		"mov r30, %0\n\t"
		: /* outputs */
		: /* inputs */  "r"(val)
		: /* clobber */
	);
}

static inline unsigned int read_r30() {
	unsigned int val;
	asm volatile (
		"mov %0, r30\n\t"
		: /* outputs */ "=r"(val)
		: /* inputs */
		: /* clobber */
	);
	return val;
}

static inline void write_r31(unsigned int val) {
	asm volatile (
		"mov r31, %0\n\t"
		: /* outputs */
		: /* inputs */  "r"(val)
		: /* clobber */
	);
}

static inline unsigned int read_r31(void) {
	unsigned int val;
	asm volatile (
		"mov %0, r31\n\t"
		: /* outputs */ "=r"(val)
		: /* inputs */
		: /* clobber */
	);
	return val;
}
