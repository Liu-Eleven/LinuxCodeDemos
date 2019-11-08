#include "../interpreter.h"
#include <stdio.h>

void fuck(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    printf("xb fuck----->%s\n", (char*)Param[0]->Val->Pointer);
	ReturnValue->Val->Integer = 123;
}





/* all math.h functions */
struct LibraryFunction DemoFunctions[] =
{
    { fuck,         "int fuck(char*);" },
    { NULL,             NULL }
};




