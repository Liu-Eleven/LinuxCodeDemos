#ifndef __INPUT_QVFB_H__
#define __INPUT_QVFB_H__


#define BUTTON_NO       0x0000
#define BUTTON_LEFT     0x0001
#define BUTTON_RIGHT    0x0002
#define BUTTON_MIDDLE   0x0004
#define BUTTON_MASK     0x00FF

typedef struct
{
  int x,y;
}POINT;


int input_qvfb_init(void);

void input_qvfb_exit(void);

int read_mouse(POINT *pt, int * button);


#endif


