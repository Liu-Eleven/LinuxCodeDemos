#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int c;

    printf("Press a key\n");
    for (;;)
    {
        c = getchar();

        printf("%c: %s letter\n", c, isalpha(c) ? "is" : "not");
        if(0x0d) 
            c = getchar();
    }
    return 0; // just to avoid warnings by compiler
}