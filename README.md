# rpi-i2s-driver
rpi-i2s-driver for rt5679(alc5680,realtek)audio codec


#编译说明

请将"arch/"和"sound/"目录下的设备树源文件和驱动源文件存放至内核源码树下，并在Kconfig和Makefile中添加相应的配置项。
配置好相关的编译项之后进行编译，得到image文件。（笔者采用的是raspberrypi build root进行编译）

编译时需要配置i2c device driver/rt5677 codec driver/i2s driver/rpi-rt5679-machine driver/alsa-lib/arecord/aplay等基本项。


#使用说明

##硬件说明：
笔者采用的是Realtek的ALC5680模组（DuerOS开发套件轻量版（Realtek）所使用模组），运行于DSP mode，
具有denoise/aec等功能，支持离线唤醒
codec的mclk由板载晶振提供，只需bcm soc提供pcm_clk(bclk) pcm_fs(frame sync signal)即可工作，
笔者为了简单起见在实现驱动的时候用的是rt5677的驱动源码，并将sample rate固定在了48kHz.


##软件说明：

采用config.txt文件替换raspberry pi的默认config.txt，启动后执行load_rt5679脚本加载相关驱动，
之后通过"aplay -l"查看声卡号和设备号,然后就可以通过aplay或者arecord进行测试使用。
