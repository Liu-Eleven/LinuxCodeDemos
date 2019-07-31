
TARGET=bmpshow
#CFLAGS= -static 

SRC=io_fb.c bmp.c main.c

$(TARGET):$(SRC)
	arm-linux-gnueabihf-gcc $(SRC)  $(CFLAGS) -o $(TARGET)

clean:
	rm $(TARGET)

install:
	cp $(TARGET) /target -f
