#pragma once

#include "common.h"

#include "base.h"

struct GpioPinData // general pattern in datasheet
{
    reg32 reserved;
    reg32 data[2];
};

struct GpioRegs
{
    reg32 func_select[6];
    struct GpioPinData output_set;
    struct GpioPindata output_clear;
    struct GpioPindata level;
    struct GpioPindata ev_detect_status;
    struct GpioPindata re_detect_enable;
    struct GpioPindata fe_detect_enable;
    struct GpioPindata hi_detect_enable;
    struct GpioPindata lo_detect_enable;
    struct GpioPindata async_re_detect;
    struct GpioPindata async_fe_detect;
    reg32 reserved;
    reg32 pupd_enable;
    reg32 pupd_enable_clocks[2];
};


#define REGS_GPIO ((struct GpioRegs *)(PBASE + 0x00200000))