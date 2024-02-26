#pragma once

#if RPI_VERSION == 4
#define PBASE 0XFE000000

#else
#error RPI_VERSION NOT DEFINED

#endif