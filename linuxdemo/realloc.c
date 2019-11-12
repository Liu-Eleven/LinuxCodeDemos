
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *pStr = "abc...[cb]";
    char *p = NULL;

    p = (char *)malloc(100);
    if (p)
        printf("Memory Allocated at: %08x\n", p);
    else
        printf("Not Enough Memory!\n");

    strcpy(p, pStr);
    printf("before value: %s\n", p);

    p = (char *)realloc(p, 256);
    if (p)
        printf("Memory Reallocated at: %08x\n", p);
    else
        printf("Not Enough Memory!\n");

    printf("after value: %s\n", p);

    free(p);

    return 0;
}