#pragma once
#include "common.h"

extern char uart_input_char_from_irq;

void irq_init_vectors();
void irq_enable();
void irq_disable();
void handle_irq();
u32 is_iir_ready();

void enable_interrupt_controller();
