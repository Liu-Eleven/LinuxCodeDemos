
TARGET=framebuffer

SRC=fbutils.c font_8x16.c font_8x8.c main.c

$(TARGET):$(SRC)
	arm-linux-gcc $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)

install:
	cp $(TARGET) /target -f
