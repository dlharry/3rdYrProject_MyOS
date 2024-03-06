#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"

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

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    timer_init();

#if RPI_VERSION == 4
    printf("\tRunning on Rpi 4\n");
#endif

    printf("\nKernel Exception Level %d\n", get_el());
    printf("\nInitialization Done\n");
    printf("\nWelcome to Hanmin's Operating System\n");

    printf("Sleeping for 200 ms...\n");
    timer_sleep(200);
    printf("Sleeping for 2000 ms...\n");
    timer_sleep(2000);
    printf("Sleeping test done.\n");

    printf("\nType something and you shall see response.\n");
    // test the uart handler
    while(1){
        uart_send(uart_recv());
    }

}