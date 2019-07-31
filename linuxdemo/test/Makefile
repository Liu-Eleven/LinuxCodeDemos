TOPDIR		:= $(shell pwd)
OUTDIR		:= $(TOPDIR)/../out

CC  := $(CROSS_COMPILE)gcc
CC  := $(CROSS_COMPILE)gcc
STRIP   := $(CROSS_COMPILE)strip
INC_DIR := ./

CFLAGS = -Wall -O2
CFLAGS +=  -I$(INC_DIR)
CXXFLAGS = 
LIBLAGS = -lpthread

OBJS := test.o
TARGET := test



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

















