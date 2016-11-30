#pragma once

/* Process the running timers
 * This is automatically called when other timer-functions are used, but needs
 * to be called at least once every 21 seconds (i.e. 2**32 cycles)
 */
void timer_tick();

/* Arm a timer to expire in <timeout> cycles, and every <interval> cycles
 * after that.
 */
void timer_arm(unsigned int timer_number, unsigned int timeout, unsigned int interval);

/* Modify a running timer
 * Either add a number of cycles to the timeout, or change the interval for the
 * next iteration.
 */
void modify_timer_timeout(unsigned int timer_number, unsigned int timeout_add);
void modify_timer_interval(unsigned int timer_number, unsigned int new_interval);

/* Returns the number of times the timer has expired since the previous call
 * (or since the start of this timer)
 * Undefined behaviour if the timer was not armed.
 */
unsigned int timer_expired(unsigned int timer_number);

/* Fiddle function to (re)set the expired counter.
 * This allows you to peek at the expired_count with timer_expired().
 */
void timer_add_expired(unsigned int timer_number, unsigned int expired_count);
