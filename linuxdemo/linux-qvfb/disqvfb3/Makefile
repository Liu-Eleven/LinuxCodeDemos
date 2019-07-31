
TARGET=disqvfb

SRC=font_8x16.c font_8x8.c disqvfb.c main.c

$(TARGET):$(SRC)
	gcc $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)

install:
	cp $(TARGET) /target -f
