KERN_DIR = /home/clay/linux/linux_4.1.15/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	$(CROSS_COMPILE)gcc -o led ledApp.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f ledtest

# 参考内核源码drivers/char/ipmi/Makefile
# 要想把a.c, b.c编译成ab.ko, 可以这样指定:
# ab-y := a.o b.o
# obj-m += ab.o

# leddrv.c board_qemu.c 编译成 led.ko
led-y := leddrv.o board_ATK.o
obj-m	+= led.o