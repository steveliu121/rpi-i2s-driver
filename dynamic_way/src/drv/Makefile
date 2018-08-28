snd-soc-rl6231-objs := rl6231.o
snd-soc-rt5677-spi-objs := rt5677-spi.o
snd-soc-rt5677-objs := rt5677.o
snd-soc-rpi-rt5677-machine-objs := rpi-rt5677-machine.o



obj-m += snd-soc-rl6231.o
obj-m += snd-soc-rt5677.o
obj-m += snd-soc-rpi-rt5677-machine.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo cp snd-soc-rl6231.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/codecs/
	sudo cp snd-soc-rt5677.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/codecs/
	sudo cp snd-soc-rpi-rt5677-machine.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/bcm/
	sudo depmod -a
