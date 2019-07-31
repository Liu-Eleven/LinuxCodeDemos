#ifndef _MSG_COM_H
#define _MSG_COM_H

#define TEXT_SZ    (2048)

struct my_msg_st{
	long int my_msg_type;
	char some_text[TEXT_SZ];
};

#endif


