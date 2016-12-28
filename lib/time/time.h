#pragma once

unsigned int time_get();
/* Get the current time
 * This needs to be called at least once every 21 seconds (2**32 cycles) to
 * avoid overflows.
 * The current time is kept as a 32bit unsigned overflowing count of clock
 * cycles
 */

/* Set of functions that compare 2 times.
 * Since times are overflowing, the comparisons are done in a ±10 second
 * (2**31 cycles) window
 * returns 0 for false; or non-zero for true
 */
inline unsigned int time_ne(unsigned int t1, unsigned int t2) {
	unsigned int ret;
	asm (
		"SUB %[ret], %[t1], %[t2]\n\t" // ret = t1 - t2
		: /* output */
			[ret] "=r" (ret)
		: /* input */
			  [t1] "r" (t1),
			[t2] "r" (t2)
		: /* clobber */
	);
	return ret;
}

inline unsigned int time_eq(unsigned int t1, unsigned int t2) {
	return ! time_ne(t1, t2);
}

inline unsigned int time_lt(unsigned int t1, unsigned int t2) {
	unsigned int ret;
	asm (
		"SUB %[ret], %[t1], %[t2]\n\t" // ret = t1 - t2
		"LSR %[ret], %[ret], 31\n\t" // keep only the "sign" bit: <0 means less-then
		: /* output */
			[ret] "=r" (ret)
		: /* input */
			[t1] "r" (t1),
			[t2] "r" (t2)
		: /* clobber */
	);
	return ret;
}

inline unsigned int time_gt(unsigned int t1, unsigned int t2) {
	return time_lt(t2, t1);
}

inline unsigned int time_le(unsigned int t1, unsigned int t2) {
	return ! time_gt(t1, t2);
}

inline unsigned int time_ge(unsigned int t1, unsigned int t2) {
	return ! time_lt(t1, t2);
}
