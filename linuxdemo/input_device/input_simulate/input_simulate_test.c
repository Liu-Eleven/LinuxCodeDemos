#include <fcntl.h>  
#include <linux/input.h>  
#include <stdio.h>  


// 查看与dev、input目录下的event对应的设备 
// cat /proc/bus/input/devices 

//要看自己的节点了  
#define EVENT_DEV "/dev/input/event4"  
  
int main(void)  
{  
    struct input_event ev;  
    int count,x,y;  
  
    int fd = open(EVENT_DEV, O_RDWR);  
    if(fd < 0){  
        printf("open %s failed\n",EVENT_DEV);  
        return 0;  
    }  
  
    while(1){  
        count = read(fd, &ev,sizeof(struct input_event));  
        if(EV_ABS == ev.type){  
            if(ev.code == ABS_X){  
                x = ev.value;  
            }else if(ev.code == ABS_Y){  
                y = ev.value;  
            }  
            printf("position: x=%d, y=%d\n",x,y);  
        }else if(EV_SYN == ev.type){  
                puts("sync!");  
        }  
    }  
  
    return 0;     
}  
