#pragma once

/* Process the running timers
 * This is automatically called when other timer-functions are used, but needs
 * to be called at least once every 21 seconds (i.e. 2**32 cycles)
 */
void timer_tick();

/* Arm a timer to expire in <timeout> cycles, and every <interval> cycles
 * after that.
 */
void timer_arm(unsigned int timer_number, unsigned int timeout, unsigned int interval=0);

/* Returns the number of times the timer has expired since the previous call
 * (or since the start of this timer)
 * Undefined behaviour if the timer was not armed.
 */
unsigned int timer_expired(unsigned int timer_number);
