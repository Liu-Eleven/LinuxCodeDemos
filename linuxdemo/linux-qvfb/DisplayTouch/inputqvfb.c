#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "inputqvfb.h"

static int mouse_fd = -1;

int read_mouse(POINT *pt, int * button)
{
    int ret1, ret2;

    ret1 = read (mouse_fd, pt, sizeof (POINT));
    ret2 = read (mouse_fd, button, sizeof (int));

    if (ret1==sizeof(POINT) && ret2==sizeof(int)&&*button<0x08)
    {
//        printf("Mouse :%d,%d  %d\n",pt->x,pt->y,*button);
        return 0;
    }
    return 1;
}

int input_qvfb_init(void)
{
    /* open mouse pipe */
    if ((mouse_fd = open ("/tmp/.qtvfb_mouse-0", O_RDONLY)) < 0)
    {
        fprintf (stderr, "can not open mouse pipe.\n");
        return 1;
    }

    return 0;
}

void input_qvfb_exit(void)
{
    close (mouse_fd);

}

