#pragma once

#include "common.h"

// MAKE LIFE EASIER
#define PBASE 0XFE000000
#define GIC_BASE  0xff840000
#define DEVICE_START 0xFC000000
#define CORE_CLOCK_SPEED 1500000000
// #define PBASE 0xffff000000000000
// #define P2V(p) ((u64)(p) + PBASE)
// #define V2P(v) ((U64)(V) - PBASE)
// #define GIC_BASE  P2V(0xff840000)