
KDIR := /home/jason/v3s/spi-nand-qt-v1/qt-spinand/linux-3.4
TARGET = input
EXEC = $(TARGET)
obj-m :=$(TARGET).o
PWD :=$(shell pwd)


all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- modules
	#arm-linux-gnueabihf-gcc $(SRC)  $(CFLAGS) -o $(TARGET_TEST)
	
clean:
	rm -rf *.o *~core.depend.*.cmd *.ko *.mod.c *.order *.symvers .tmp_versions $(TARGET) 
#	rm -rf $(TARGET_TEST)


	