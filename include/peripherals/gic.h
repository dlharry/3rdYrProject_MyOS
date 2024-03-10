#pragma once

#include "base.h"
#include "common.h"

struct GicData // general pattern in datasheet
{
    reg32 data[15];
    reg32 reserved[17];
};

struct arm_gicd_regs_2711
{
    reg32 control;
    reg32 type;
    reg32 imple_id;
    reg32 reserved1[30];
    struct GicData group;
    struct GicData isenable;
    struct GicData icenable;
    struct GicData set_pending;
    struct GicData clear_pending;
    struct GicData set_active;
    struct GicData clear_active;
    reg32 priority[63];
    reg32 reserved2[129];
    reg32 proccessor_targets[63];
    reg32 reserved3[129];
    reg32 configuration[31];
};
struct arm_gicc_regs_2711
{
    reg32 control;
    reg32 priority_mask;
    reg32 binary_piont;
    reg32 interrupt_acknowlage;
    reg32 end_of_interrupt;
};


#define REGS_GICD ((arm_gic_regs_2711 *)(GIC_BASE + 0x1000))
#define REGS_GICC ((arm_gic_regs_2711 *)(GIC_BASE + 0x2000))
