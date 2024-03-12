#pragma once

#include "utils.h"

void uart_init();
char uart_recv();
void uart_send(char c);
void uart_send_string(char *str);
u32 uart_isReadByteReady();
u32 uart_isWriteByteReady();
void uart_loadOutputFifo();
void uart_update();
u32 uart_isOutputQueueEmpty();


#define UART_MAX_QUEUE  16 * 1024