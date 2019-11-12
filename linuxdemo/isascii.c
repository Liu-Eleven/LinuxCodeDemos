#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

// ascii码指0x00-0x7F之间的字符

int main(int argc, char *argv[])
{
    char s[] = "!@#$%^&*()abcdef+++----BJGGV\a\a";
    int i = 0;
    int size = sizeof(s) / sizeof(char); // length of string s

    printf("size:%d %s\n", size, s);
    s[0] = -1;
    s[1] = -2;
    s[2] = -3;
    for (i = 0; i < size; i++)
    {
        printf("0x%02x,", s[i]);
    }

    printf("\n");

    for (i = 0; i < size; i++)
    {
        if (isascii(s[i]))
            putchar('^');
        else
            putchar('.');
    }
    printf("\n");
    return 0;
}