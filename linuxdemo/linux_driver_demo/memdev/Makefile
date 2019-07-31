ifneq ($(KERNELRELEASE),)

obj-m := memdev.o

else
	
KDIR := /home/guoqian/4-3-1/linux-2.6.29
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*

endif
