#KERNEL_DIR	:= /lib/modules/$(shell uname -r)/build
KERNEL_DIR	:= /home/jason/work/CQA83Tubuntu_Mate_bv3/linux-3.4
KERNEL_VERSION	:= $(shell uname -r)
PWD		:= $(shell pwd)
MODULE_NAME := sr9800
obj-m		:= $(MODULE_NAME).o

all: 
	@echo "Building SR9800 USB 2.0 USB2NET chip driver..."
	@(cd $(KERNEL_DIR) && make -C $(KERNEL_DIR) SUBDIRS=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- modules)

clean:
	-rm -f *.o *.ko .*.cmd .*.flags .mii.mod.o.d *.mod.c Module.symvers Module.markers modules.order version.h
	-rm -rf .tmp_versions

