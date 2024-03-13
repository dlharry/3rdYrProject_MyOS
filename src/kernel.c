#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"
#include "fb.h"
#include "breakout.h"

char uart_input_char_from_irq;

// To support Kustaa Nyholm's printf
void putc(void *p, char c){
    if(c == '\n'){
        uart_send('\r');
    }

    uart_send(c);
}

u32 get_el();

void kernel_main(){
    uart_init();
    init_printf(0, putc);
    printf("UART initiated...\nHello My OS!\n");

    uart_input_char_from_irq = 0;

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    timer_init();
    fb_init();


#if RPI_VERSION == 4
    printf("\tRunning on Rpi 4\n");
#endif

    printf("\nKernel Exception Level %d\n", get_el());
    printf("\nKernel Initialization Done\n");
    printf("\nWelcome to Hanmin's 'Operating System'\n");
    printf("\nType something and you shall see response.\n");

    printf("Sleeping for 200 ms...\n");
    timer_sleep(200);
    printf("Sleeping for 2000 ms...\n");
    timer_sleep(2000);
    printf("Sleeping test done.\n");

    printf("game Initializing\n");
    breakout();
}