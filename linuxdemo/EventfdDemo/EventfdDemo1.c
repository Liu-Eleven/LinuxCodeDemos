
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <stdint.h>         /* Definition of uint64_t */

#define handle_error(msg)   \
    do { perror(msg); exit(EXIT_FAILURE);  } while(0)

int main(int argc, char **argv)
{
    int efd, i;

    uint64_t u;
    ssize_t rc;

    if(argc < 2){
        fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    efd = eventfd(0, 0);
    if(efd == -1)
        handle_error("eventfd");

    switch(fork()){
        case 0:
            for(i=1; i<argc; i++){
                printf("Child writing %s to efd\n",argv[i]);
                u = atoll(argv[i]);
                rc = write(efd, &u, sizeof(uint64_t));
                if(rc != sizeof(uint64_t))
                    handle_error("write");
            }
            printf("Child completed write loop\n");
			printf("Child exit\n");
            exit(EXIT_SUCCESS);
        default:
            sleep(2);

            printf("Parent about to read\n");
            rc = read(efd, &u, sizeof(uint64_t));
            if(rc != sizeof(uint64_t))
                handle_error("read");

            printf("Parent read %llu from efd\n",(unsigned long long)u);
        case -1:
            handle_error("fork");
    }
    return 0;
}



