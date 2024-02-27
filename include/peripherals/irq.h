// irq for user
// 2.7.11 BCM2711

#pragma once

#include "peripherals/base.h"
#include "common.h"

enum vc_irqs { // receive the update interrputs
    AUX_IRQ = (1 << 29)
};

struct arm_irq_regs_2711 { // rpi4, different from rpi3
    reg32 irq0_pending_0;
    reg32 irq0_pending_1;
    reg32 irq0_pending_2;
    reg32 res0;
    reg32 irq0_enable_0;
    reg32 irq0_enable_1;
    reg32 irq0_enable_2;
    reg32 res1;
    reg32 irq0_disable_0;
    reg32 irq0_disable_1;
    reg32 irq0_disable_2;
};

#if RPI_VERSION == 4
    typedef struct arm_irq_regs_2711 arm_irq_regs;
#else
    typedef struct arm_irq_regs_2711 arm_irq_regs;
#endif

#define REGS_IRQ ((arm_irq_regs *)(PBASE + 0x0000B200))
