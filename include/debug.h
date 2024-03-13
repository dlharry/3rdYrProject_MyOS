#include "utils.h"

#define ASSERT(e) do {                          \
    if(!(e)) error_check(__FILE__, __LINE__);   \
} while (0)

void error_check(char* file, u64 line);