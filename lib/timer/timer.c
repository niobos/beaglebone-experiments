#include "timer.h"

#include <pru/ctrl.h>

#ifndef NUMBER_OF_TIMERS
#error Please define the number of timers needed
#endif

struct timer {
	enum {TIMER_DISABLED, TIMER_RUNNING} state;
	unsigned int interval;
	unsigned int expire;
	unsigned int expired_count;
} timers[NUMBER_OF_TIMERS];

void timer_init() {
	for(unsigned int i=0; i < NUMBER_OF_TIMERS; i++) {
		timers[i].state = TIMER_DISABLED;
	}
	timer_tick();
}

void timer_tick() {
	register unsigned int old_count, temp;
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
	 * was disabled
	 */
	old_count += 11;

	for(unsigned int i=0; i < NUMBER_OF_TIMERS; i++) {
		if( timers[i].state == TIMER_DISABLED) continue;

		while( timers[i].expire <= old_count ) {
			timers[i].expired_count++;
			timers[i].expire += timers[i].interval;

			if( timers[i].interval == 0 ) {
				// one-shot timer
				timers[i].state = TIMER_DISABLED;
				break;
			}
		}

		timers[i].expire -= old_count;
	}
}

void timer_arm(
	unsigned int timer_number,
	unsigned int timeout,
	unsigned int interval
) {
	unsigned int now = PRU_THIS_CTRL.CYCLECOUNT;
	timers[timer_number].state = TIMER_RUNNING;
	timers[timer_number].expire = now + timeout;
	timers[timer_number].interval = interval;
	timers[timer_number].expired_count = 0;

	timer_tick();
}

unsigned int timer_expired(unsigned int timer_number) {
	timer_tick();

	unsigned int e = timers[timer_number].expired_count;
	timers[timer_number].expired_count = 0;
	return e;
}