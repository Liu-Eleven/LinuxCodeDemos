#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "disqvfb.h"
#include "inputqvfb.h"

int main(void)
{
  POINT point;
  int button;
  char str[50];

  if(dis_qvfb_init()!=0)
  {
    printf("Qvfb Init Error\n");
    return 1;
  }

  if(input_qvfb_init()!=0)
  {
    return 1;
  }

  qvfb_clear(0);
  put_string_center(120,200,"by LastRitter",90000);
  put_string_center(120,220,"superyongzhe@163.com",90000);
  
  while(1)
  {
    if(read_mouse(&point,&button)!=0)
      continue;
    sprintf(str,"Mouse :%3d,%3d %s",point.x,point.y,button?"Down":" Up ");
    fillrect(10,80,230,120,0);
    put_string_center(120,100,str,50000);
    qvfb_update(10,80,230,120);
  }

  dis_qvfb_exit();
  input_qvfb_exit();

  return 0;
}
