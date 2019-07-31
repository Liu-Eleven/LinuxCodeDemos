#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main (int args, char* arg[])
{
    int i;
    int fd;
    char* mem;
    char *buff = "HELLO";
    //open /dev/mem with read and write mode
    if((fd = open ("/dev/mem", O_RDWR)) < 0)
    {
        perror ("open error");
        return -1;
    }

    //map physical memory 0-10 bytes
    mem = mmap (0, 10, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED)
    {
        perror ("mmap error:");
        return 1;
    }

    //Read old value
    for (i = 0; i < 5; i++)
    {
        printf("\nold mem[%d]:%d", i, mem[i]);
    }

    printf("The value is 0x%x\n", *((int *)mem));

    //write memory
    memcpy(mem, buff, 5);
    //Read new value
    for (i = 0; i<5 ; i++)
    {
        printf("\nnew mem[%d]:%c", i, mem[i]);
    }

    printf("\n");
    munmap (mem, 10); //destroy map memory
    close (fd);   //close file
    return 0;
}
