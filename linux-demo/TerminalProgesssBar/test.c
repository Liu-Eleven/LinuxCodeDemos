
#include "TerminalProgesssBar.h"
#include <unistd.h>

int main(int argc, char *argv[] )
{
    progress_t bar;
    //progress_init(&bar, "", 50, PROGRESS_NUM_STYLE);
    //progress_init(&bar, "", 50, PROGRESS_CHR_STYLE);
    progress_init(&bar, "", 50, PROGRESS_BGC_STYLE);

    int i;
    for ( i = 0; i <= 50; i++ ) {
	progress_show(&bar, i/50.0f);
	//sleep(1);
	usleep(100000); //Nus
    }
    printf("\n+-Done\n");

    progress_destroy(&bar);

    return 0;
}
