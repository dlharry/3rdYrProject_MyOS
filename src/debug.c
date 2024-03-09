#include "debug.h"
#include "printf.h"

void error_check(char* file, u64 line){
    printf("\n\n-------------------------------\n\n");
    printf("|            ERROR CHECK          |");
    printf("\n\n-------------------------------\n\n");
    printf("Assertion Fail [%s: %u]\n\n",file, line);

    while(1);
}