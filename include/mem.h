#pragma once

#include "common.h"

// void *memcpy(void *dest, const void *src, u32 n);
void memset(void *dst, int value, u32 size);
void memcpy(void *dst, void *src, u32 size);
void memmove(void *dst, void *src, u32 size);
int memcmp(void *src1, void *src2, u32 size);

#define GPU_CACHED_BASE		0x40000000
#define GPU_UNCACHED_BASE	0xC0000000
#define GPU_MEM_BASE	GPU_UNCACHED_BASE

#define BUS_ADDRESS(addr)	(((addr) & ~0xC0000000) | GPU_MEM_BASE)