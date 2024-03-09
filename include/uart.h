#pragma once

#include "peripherals/uart.h"

char read_char();
void init_uart();
void write_char(char c);
void write_string(const char *str);
