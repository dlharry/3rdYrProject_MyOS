#pragma once

#if RPI_VERSION == 4
#define PBASE 0XFE000000

#else
#define PBASE 0
#error RPI_VERSION NOT DEFINED

#endif