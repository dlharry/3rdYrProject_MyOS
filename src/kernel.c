#include "common.h"
#include "mini_uart.h"

void kernel_main(){
    uart_init();
    uart_send_string("Hello My OS!\n");

#if RPI_VERSION == 4
    uart_send_string("\tRunning on Rpi 4\n");
#endif

    uart_send_string("\nInitialization Done\n");

    while(1){
        uart_send(uart_recv());
    }

}