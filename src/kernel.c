#include "common.h"
#include "mini_uart.h"
#include "printf.h"

// To support Kustaa Nyholm's printf
void putc(void *p, char c){
    if(c == '\n'){
        uart_send('\r');
    }

    uart_send(c);
}

void kernel_main(){
    uart_init();
    init_printf(0, putc);
    printf("Hello My OS!\n");

#if RPI_VERSION == 4
    printf("\tRunning on Rpi 4\n");
#endif

    printf("\nInitialization Done\n");

    while(1){
        uart_send(uart_recv());
    }

}