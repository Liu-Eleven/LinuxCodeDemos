#ifndef __DIS_QVFB_H__
#define __DIS_QVFB_H__


typedef struct {
  int left,top,right,bottom;
}RECT;

typedef unsigned char BYTE;

typedef struct
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    RECT update;
    BYTE dirty;
    int  numcols;
    unsigned int clut[256];
}QVFbHeader;


int dis_qvfb_init(void);


void pixel (int x, int y, int colidx);

void line (int x1, int y1, int x2, int y2, int colidx);

void qvfb_clear(int color);

void rect (int x1, int y1, int x2, int y2, int colidx);

void fillrect (int x1, int y1, int x2, int y2, int colidx);


void put_string(int x, int y, char *s, int color);

void put_string_center(int x, int y, char *s, int color);


void qvfb_update(int left,int top,int right,int bottom);
void dis_qvfb_exit(void);


#endif


