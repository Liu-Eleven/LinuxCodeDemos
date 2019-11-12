
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int c;

    c = 'a';
    printf("%c:%s\n", c, isalnum(c) ? "yes" : "no");
    c = '7';
    printf("%c:%s\n", c, isalnum(c) ? "yes" : "no");
    c = '@';
    printf("%c:%s\n", c, isalnum(c) ? "yes" : "no");
    getchar();
    
    return 0;
}