#include "utils.h"
#include "uart.h"

void write_char(char c) {
    while(REGS_UART->FR & (1<<5));
    REGS_UART->DR = c;
}

char read_char() {
    while(REGS_UART->FR & (1<<4));
    return REGS_UART->DR;
}

void write_string(const char *str) {
    while(*str) {
        if(*str == '\n'){
            write_char('\r');
        }
        write_char(*str);
        str++;
    }
}

void init_uart(void) {
    REGS_UART->CR = 0;
    REGS_UART->IBRD = 26;
    REGS_UART->FBRD = 0;
    REGS_UART->LCRH = (1<<4) | (1<<5) | (1<<6);
    REGS_UART->IMSC = 0;
    REGS_UART->CR = (1<<0) | (1<<8) | (1<<9);
}