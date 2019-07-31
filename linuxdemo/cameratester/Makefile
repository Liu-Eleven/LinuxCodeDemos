TOPDIR		:= $(shell pwd)
OUTDIR		:= $(TOPDIR)/bin

export CROSS_COMPILE=arm-linux-gnueabi-

CC  := $(CROSS_COMPILE)gcc
CC  := $(CROSS_COMPILE)gcc
STRIP   := $(CROSS_COMPILE)strip
INC_DIR := ./

CFLAGS = -Wall -O2 -static
CFLAGS +=  -I$(INC_DIR)
CXXFLAGS = 
LIBLAGS = -lpthread -ljpeg

OBJS := cdecode.o \
	cencode.o \
	MainServer.o \
	jpeg.o \
	camera.o \
	convert.o
	
	
TARGET := NailPrinterServer


all:$(TARGET) $(OUTDIR)
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(TYPEFLAGS) $(SRCFILES) $(LOCALFLAGS) $? -o $@ $(LIBLAGS)
	$(STRIP) $@

$(OUTDIR):
	mkdir -p $@

install:
	cp $(TARGET) $(OUTDIR)

clean:
	rm -rf $(OBJS) $(TARGET) *~

.PHONY: all install clean

















