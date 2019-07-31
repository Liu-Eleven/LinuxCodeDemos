#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "fbutils.h"

static int palette [] =
{
	0x000000, 0xffe080, 0xffffff, 0xe0c0a0, 0x304050, 0x80b8c0
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))

struct ts_button {
	int x, y, w, h;
	char *text;
	int flags;
#define BUTTON_ACTIVE 0x00000001
};

/* [inactive] border fill text [active] border fill text */
static int button_palette [6] =
{
	1, 4, 2,
	1, 5, 0
};

#define NR_BUTTONS 2
static struct ts_button buttons [NR_BUTTONS];

static void sig(int sig)
{
	close_framebuffer();
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);
	exit(1);
}

static void button_draw (struct ts_button *button)
{
	int s = (button->flags & BUTTON_ACTIVE) ? 3 : 0;
	rect (button->x, button->y, button->x + button->w - 1,
	      button->y + button->h - 1, button_palette [s]);
	fillrect (button->x + 1, button->y + 1,
		  button->x + button->w - 2,
		  button->y + button->h - 2, button_palette [s + 1]);
	put_string_center (button->x + button->w / 2,
			   button->y + button->h / 2,
			   button->text, button_palette [s + 2]);
}


int main()
{
	int i,x,y;

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);


	if (open_framebuffer()) {
		close_framebuffer();
		exit(1);
	}

	x = xres/2;
	y = yres/2;

	for (i = 0; i < NR_COLORS; i++)
		setcolor (i, palette [i]);

	fillrect (0,0,xres-1,yres-1,0);

	/* Initialize buttons */
	memset (&buttons, 0, sizeof (buttons));

	buttons [0].w = xres / 4;
	buttons [0].h = 20;
	buttons [0].x = xres / 4 - buttons [0].w / 2;
	buttons [0].y = buttons [1].y = 10;
	buttons [0].text = "Left";

	buttons [1].w = xres / 4;
	buttons [1].h = 20;
	buttons [1].y = 10;
	buttons [1].x = (3 * xres) / 4 - buttons [0].w / 2;
	buttons [1].text = "Right";

	for (i = 0; i < NR_BUTTONS; i++)
		button_draw (&buttons [i]);

	put_string_center (xres/2, yres/2-10,"Frame Buffer Test", 2);
	put_string_center (xres/2, yres/2+10,"  by LastRitter", 2);

    	line (10,300,200,320, 2);

        getchar();

        close_framebuffer();

}

