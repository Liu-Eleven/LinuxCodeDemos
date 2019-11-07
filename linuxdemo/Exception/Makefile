objects = Exception.o test.o

CC = g++
demo : $(objects)
	$(CC) -o demo $(objects)

Exception.o : Exception.cpp
	$(CC) -c $(CCFLAGS) Exception.cpp
test.o : test.cpp
	$(CC) -c $(CCFLAGS) test.cpp

	
.PHONY : clean
clean :
	   rm -f demo $(objects) 
