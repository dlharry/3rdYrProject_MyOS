#include "gpio.h"
#include "utils.h"
#include "peripherals/aux.h"
#include "mini_uart.h"
#include "printf.h"

#define TXD 14
#define RXD 15

unsigned char uart_output_queue[UART_MAX_QUEUE];
unsigned int uart_output_queue_write = 0;
unsigned int uart_output_queue_read = 0;

void uart_init(){
    gpio_pin_set_func(TXD, GFAlt5);
    gpio_pin_set_func(RXD, GFAlt5);

    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_cntr = 0;
    REGS_AUX->mu_ier = 0xD;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;

#if RPI_VERSION == 4
    REGS_AUX->mu_baud = 541; // = 115200 @ 500 Mhz
#endif

    REGS_AUX->mu_cntr = 3;

    // uart_send('\r'); // test 
    // uart_send('\n');
    // uart_send('\n');
}

void uart_send(char c){
    while(!(uart_isWriteByteReady())); // loop until lsr set

    REGS_AUX->mu_io = c;
}

void uart_send_ByteBlocking(char c){
    unsigned int next = (uart_output_queue_write + 1) & (UART_MAX_QUEUE - 1); // Don't overrun

    while (next == uart_output_queue_read) uart_loadOutputFifo();

    uart_output_queue[uart_output_queue_write] = c;
    uart_output_queue_write = next;
}

void uart_drainOutputQueue() {
    while (!uart_isOutputQueueEmpty()) uart_loadOutputFifo();
}

char uart_recv(){
    while(!(uart_isReadByteReady())); // loop until lsr set

    // return REGS_AUX->mu_io & 0xFF;
    printf("uartoutputqueue:[ %s ]\n", uart_output_queue);
    return REGS_AUX->mu_io;
}

void uart_send_string(char *str){
    while(*str){
        if(*str == '\n'){
            uart_send('\r');
        }
        uart_send(*str);
        str++;
    }
}

u32 uart_isOutputQueueEmpty() {
    return uart_output_queue_read == uart_output_queue_write;
}

u32 uart_isReadByteReady()  { return REGS_AUX->mu_lsr & 0x01; }
u32 uart_isWriteByteReady() { return REGS_AUX->mu_lsr & 0x20; }


void uart_loadOutputFifo() {
    while (!uart_isOutputQueueEmpty() && uart_isWriteByteReady()) {
        uart_send(uart_output_queue[uart_output_queue_read]);
        uart_output_queue_read = (uart_output_queue_read + 1) & (UART_MAX_QUEUE - 1); // Don't overrun
    }
}

void uart_update() {
    uart_loadOutputFifo();

    if (uart_isReadByteReady()) {
       unsigned char ch = uart_recv();
       if (ch == '\r') uart_send_string("\n"); else uart_send_ByteBlocking(ch);
    }
}