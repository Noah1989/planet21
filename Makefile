all: planet.ihx

objects = crt21.rel gpu.rel text.rel planet.rel

planet.ihx: $(objects)
	sdcc --Werror -mz80 --code-loc 0x8000 --data-loc 0xF000 -o $@ --no-std-crt0 $(objects)

%.rel: %.c *.h
	sdcc --Werror -mz80 --opt-code-size -c $<

%.rel: %.s
	sdasz80 -o $<

clean:
	rm -vf *.rel *.asm *.ihx *.lk *.lst *.map *.noi *.sym *.adb *.cdb

.SECONDARY:
.PHONY: clean all
