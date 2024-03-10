#include "common.h"
#include "mini_uart.h"
#include "uart.h"
#include "debug.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"
#include "mailbox.h"
#include "video.h"
#include "mem.h"

// To support Kustaa Nyholm's printf
void putc(void *p, char c){
    if(c == '\n'){
        uart_send('\r');
    }

    uart_send(c);
}

u32 get_el();

struct align_check1 {
    u8 a;
    u8 b;
    u8 c;
    //u8 padding;
    u32 d;
};

struct align_check2 {
    u8 a;
    u8 b;
    u8 c;
    u32 d;
} PACKED;

u8 buffer[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};

void kernel_main(){
    uart_init(); // mini uart
    init_uart(); // uart
    init_printf(0, putc);
    printf("UART initiated...\nHello My OS!\n");

    ASSERT(1); // test assert

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    // test Data abort exception
    // char *p = (char*)0xffff000000000000; 
    // *p = 1;
    // printf("This message should not be printed\n");

    timer_init();

#if RPI_VERSION == 4
    printf("\tRunning on Rpi 4\n");
#endif

#if INIT_MMU == 1
    printf("Initialized MMU\n");
#endif

    printf("ALIGN CHECK\n");

    struct align_check1 ac1;
    memcpy(&ac1, buffer, 7);
    printf("UNPACKED: A: %X, B: %X, C: %X, D: %X\n", ac1.a, ac1.b, ac1.c, ac1.d);

    struct align_check2 ac2;
    memcpy(&ac2, buffer, 7);
    printf("PACKED: A: %X, B: %X, C: %X, D: %X\n", ac2.a, ac2.b, ac2.c, ac2.d);

    void *p1 = get_free_pages(10);
    void *p2 = get_free_pages(4);
    void *p3 = allocate_memory(20 * 4096 + 1);

    free_memory(p1);
    free_memory(p2);
    free_memory(p3);

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