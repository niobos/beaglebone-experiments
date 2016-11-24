#include <pru/cfg.h>
#include <am335x/gpio.h>

#include "resource_table.h"

const unsigned int CPU_FREQ = 200 * 1000 * 1000;

inline static void busy_wait_us(const unsigned int us) {
	__delay_cycles( us * (CPU_FREQ / (1000*1000)) );
}

const unsigned int period_us = 500 * 1000;

int main() {

	// Enable the OCP master port
	// This is needed to access external memory
	PRU_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	while(1) {
		GPIO1.SETDATAOUT = 1<<21;
		busy_wait_us(period_us);
		GPIO1.CLEARDATAOUT = 1<<21;
		busy_wait_us(period_us);
	}

	__builtin_unreachable();
}
