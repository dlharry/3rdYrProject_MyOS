#pragma once

#include "common.h"

// void *memcpy(void *dest, const void *src, u32 n);
void memset(void *dst, int value, u32 size);
void memcpy(void *dst, void *src, u32 size);
void memmove(void *dst, void *src, u32 size);
int memcmp(void *src1, void *src2, u32 size);