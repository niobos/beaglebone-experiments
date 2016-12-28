#include "time.h"

#include <pru/ctrl.h>

static unsigned int time_now;

unsigned int time_get() {
	unsigned int old_count, temp;
	asm volatile (
		// Disable counter
		"LBBO %[temp], %[ctrl_reg], 0, 4\n\t"                        // 4 cycles
		"CLR %[temp], %[temp], 3\n\t"                                // 1 cycle
		"SBBO %[temp], %[ctrl_reg], 0, 4\n\t"                        // 3 cycles; counter stops

		// Reset counter
		"LBBO %[old_count], %[ctrl_reg], 0xc, 4\n\t"                 // 4 cycles
		// Actual value stored is ignored, counter is reset to 0 on any write
		"SBBO %[old_count], %[ctrl_reg], 0xc, 4\n\t"                 // 3 cycles

		// Re-enable counter
		"SET %[temp], %[temp], 3\n\t"                                // 1 cycle
		"SBBO %[temp], %[ctrl_reg], 0, 4\n\t"                        // 3 cycles; counter starts

		: /* outputs */
			[old_count] "=&r" (old_count),
			[temp] "=&r" (temp)
		: /* inputs */
			[ctrl_reg] "r" (&PRU_THIS_CTRL)
		: /* clobber */
			/* PRU_THIS_CTRL.CYCLECOUNT is declared volatile, so we don't need to
			 * mention it explicitly here */
	);

	/* correct timing for the cycles we missed during the time that the counter
	 * was disabled */
	old_count += 11;

	time_now += old_count;
	return time_now;
}
