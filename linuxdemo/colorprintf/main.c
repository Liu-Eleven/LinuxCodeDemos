
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "colorprintf.h"

/*
 * Output colored text to ANSI (Linux, Mac, iPhone) and Windows terminals.
 * This function wraps printf (by using vprintf), and adds a 'color' parameter to it.
 *
 * color = -1 -> not colored (same as printf)
 *          0 -> red
 *          1 -> green
 *          2 -> yellow
 *          3 -> blue
 *          4 -> magenta
 *          5 -> cyan
 */

int main(int argc, char **argv)
{
	colorprintf(-1, "test line!\n");
	colorprintf(0, "test line!\n");
	colorprintf(1, "test line!\n");
	colorprintf(2, "test line!\n");
	colorprintf(3, "test line!\n");
	colorprintf(4, "test line!\n");
	colorprintf(5, "test line!\n");
	return 0;
}
