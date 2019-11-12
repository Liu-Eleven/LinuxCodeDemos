#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  char *p = NULL;

  p = (char *)calloc(100, sizeof(char));
  if (p)
    printf("Memory Allocated at: %08x\n", p);
  else
    printf("Not Enough Memory!\n");

  free(p);

  //getchar();
  return 0;
}
