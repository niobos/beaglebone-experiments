%.0.elf: %.S
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru0 -nostdlib -nodefaultlibs -nostartfiles $< -o $@

%.1.elf: %.S
	pru-gcc -g -Os -Wall -Wextra -minrt -mmcu=am335x.pru1 -nostdlib -nodefaultlibs -nostartfiles $< -o $@

%.bin: %.elf
	pru-objcopy -O binary -j .text $< $@
