TARGET = $(notdir $(CURDIR))

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

SRCS = $(wildcard *.c) $(wildcard board_cfg_bds/*.c)

OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY: clean install prep test

all: prep $(TARGET)

prep:
	@echo "target: $(TARGET)"
	@git log | sed -n 1p | awk '{print $$2}'| sed 's/^/"&/g' | sed 's/$$/&"/g' > git_info.git

$(TARGET): $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) netman_version.svn netman_version.git

install: $(TARGET)
	@if [ -n "$(INSTALL_DIR)" ];then \
		mkdir -p $(INSTALL_DIR); \
		cp $(TARGET) $(INSTALL_DIR); \
	fi

help:
	@echo "No help now!"

test:
	@echo "test: $(OBJS)"


