#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>
#include <unistd.h>


#include "Exception.h"
#include <functional>
#include <vector>
#include <stdio.h>




void func()
{
	printf("Stack:\n%s\n", getStackTrace(true).c_str());
	
	printf("\n\n---------------------------------------\n\n");
	
	printf("Stack:\n%s\n", getStackTrace(false).c_str());
	
	printf("\n\n---------------------------------------\n\n");
	
	throw Exception("oops");
	//*((volatile char *)0x0) = 0x9999;
}

int main(int argc, char *argv[])
{
	try
    {
      func();
    }
    catch (const Exception& ex)
    {
		fprintf(stderr, "in catchblock......\n");
		fprintf(stderr, "reason: %s\n", ex.what());
		fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
		abort();
    }
    catch (const std::exception& ex)
    {
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
    }
    catch (...)
    {
		fprintf(stderr, "unknown exception caught!\n");
		throw; // rethrow
    }
	
	printf("---------< 1 >-------------!\n");
	
	func();
	
	printf("---------< 2 >-------------!\n");

	return 0;
}
