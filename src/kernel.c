#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"
#include "mailbox.h"
#include "video.h"

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

    printf("MAILBOX:\n");
    printf("CORE CLOCK: %d\n", mailbox_clock_rate(CT_CORE));
    printf("EMMC CLOCK: %d\n", mailbox_clock_rate(CT_EMMC));
    printf("UART CLOCK: %d\n", mailbox_clock_rate(CT_UART));
    printf("ARM CLOCK: %d\n", mailbox_clock_rate(CT_ARM));

    printf("I2C POWER STATE:\n");

    for(int i=0; i<3; ++i){
        bool on = mailbox_power_check(i);

        printf("POWER DOMAIN STATUS FOR %d = %d\n", i, on);
    }

    u32 max_temp = 0;

    mailbox_generic_command(RPI_FIRMWARE_GET_MAX_TEMPERATURE, 0, &max_temp);
    
    // hdmi video testing
    video_init();

    printf("WITHOUT DMA...\n");
    video_set_dma(false);
    // 32 bit
    printf("Resolution 1900x1200\n");
    video_set_resolution(1900, 1200, 32);
    printf("Resolution 1024x768\n");
    video_set_resolution(1024, 768, 32);
    printf("Resolution 800x600\n");
    video_set_resolution(800, 600, 32);
    // 8 bit
    printf("Resolution 1900x1200\n");
    video_set_resolution(1900, 1200, 8);
    printf("Resolution 1024x768\n");
    video_set_resolution(1024, 768, 8);
    printf("Resolution 800x600\n");
    video_set_resolution(800, 600, 8);

    printf("WITH DMA...\n");
    video_set_dma(true);
    //32 bit
    printf("Resolution 1900x1200\n");
    video_set_resolution(1900, 1200, 32);
    printf("Resolution 1024x768\n");
    video_set_resolution(1024, 768, 32);
    printf("Resolution 800x600\n");
    video_set_resolution(800, 600, 32);
    printf("Resolution 1900x1200\n");
    // 8 bit
    video_set_resolution(1900, 1200, 8);
    printf("Resolution 1024x768\n");
    video_set_resolution(1024, 768, 8);
    printf("Resolution 800x600\n");
    video_set_resolution(800, 600, 8);

    while(1) {
        u32 cur_temp = 0;
        mailbox_generic_command(RPI_FIRMWARE_GET_TEMPERATURE, 0, &cur_temp);
        printf("Cur temp: %dC MAX: %dC\n", cur_temp/1000, max_temp/1000);
        timer_sleep(1000);
    }

    printf("\nType something and you shall see response.\n");
    // test the uart handler
    while(1){
        uart_send(uart_recv());
    }

}