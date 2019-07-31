#ifndef DEFINECMD
#define DEFINECMD


/*
* command type used 
*/
#define COMMAND_OPERATION    ('1') // command such as printer
#define SEND_FILE    ('2') // send file from server to client
#define RECEIVE_FILE    ('3') // send file from client to server 



/*
* communication status
*/
#define READY    ('1') 



#define ARGC_MAX    (8)

typedef struct tagArg
{
	int argc;
	char argv[ARGC_MAX][64];
}tArg;



#endif




