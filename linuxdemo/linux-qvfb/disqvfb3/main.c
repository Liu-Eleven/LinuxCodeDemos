#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "disqvfb.h"


int main(void)
{


  if(dis_qvfb_init()!=0)
  {
    printf("Qvfb Init Error\n");
    return 1;
  }

  qvfb_clear(90000);

  fillrect(20,20,200,300,30000);
  rect(100,200,200,300,40000);
  line(70,40,230,300,20000);
  put_string_center(120,100,"Qvfb Test!",50000);
  put_string_center(120,200,"LastRitter",90000);
  qvfb_update(20,20,220,300);

  dis_qvfb_exit();

  return 0;
}
