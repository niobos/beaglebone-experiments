%.0.elf: %.S
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru0 -nostdlib -nodefaultlibs -nostartfiles $< -o $@

%.1.elf: %.S
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru1 -nostdlib -nodefaultlibs -nostartfiles $< -o $@

%.bin: %.elf
	pru-objcopy -O binary -j .text $< $@

%.0.elf: %.c
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru0 -I include $< -o $@

%.1.elf: %.c
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru1 -I include $< -o $@

02_rpmsg.0.elf: 02_rpmsg.c lib/rpmsg/rpmsg.c lib/timer/timer.c
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru0 -DNUMBER_OF_TIMERS=1 -I include $+ -o $@
