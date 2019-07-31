
#include<setjmp.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct _tagExceptionSign {
	jmp_buf _stackInfo;
	int _exceptype;
}tagExceptionSign;


#define ExcepType(ExcepSign) ((ExcepSign)._exceptype)

#define Try(ExcepSign) if(((ExcepSign)._exceptype = setjmp((ExcepSign)._stackInfo)) == 0)

#define Catch(ExcepSign, ExcepType) else if((ExcepSign)._exceptype == ExcepType)
	
#define Finally 

#define Throw(ExcepSign, ExcepType) longjmp((ExcepSign)._stackInfo, ExcepType)


void ExceptionTest(int expType) {
	tagExceptionSign ex;
	expType = expType < 0 ? -expType : expType;
	Try(ex) {
		if(expType > 0) {
			Throw(ex, expType);
		} else {
			printf("no exception\n");
		}
	} Catch(ex, 1) {
		printf("exception 1\n");
	} Catch(ex, 2) {
		printf("exception 2\n");
	} Finally {
		printf("other\n");
	}
} 

int main() {
	ExceptionTest(1);
	ExceptionTest(2);
	ExceptionTest(0);
	
	return 0;
}






