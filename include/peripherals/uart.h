#pragma once

#include "common.h"

#include "peripherals/base.h"

struct UartRegs
{
    reg32 DR;
    reg32 reserved[5];
    reg32 FR;
    reg32 ILPR;
    reg32 IBRD;
    reg32 FBRD;
    reg32 LCRH;
    reg32 CR;
    reg32 IFLS;
    reg32 IMSC;
    reg32 RIS;
    reg32 MIS;
    reg32 ICR;
    reg32 DMACR;
};


#define REGS_UART ((struct UartRegs *)(PBASE + 0x00201000))