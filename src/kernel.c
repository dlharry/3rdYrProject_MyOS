#include "common.h"
#include "mini_uart.h"
#include "printf.h"
#include "debug.h"
#include "irq.h"
#include "timer.h"
#include "fb.h"
#include "breakout.h"
#include "mmu.h"
#include "mem.h"
#include "mm.h"
#include "print_to_screen.h"

char uart_input_char_from_irq; // Passing uart interrupt catching characters
int output_loc_x;
int output_loc_y;

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
    uart_init();
    init_printf(0, putc);
    printf("UART initiated...\nHello My OS!\n");
    ASSERT(1); // test assert

    uart_input_char_from_irq = 0;
    output_loc_x = 100;
    output_loc_y = 100;

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    printf("IRQ initiated\n");
    timer_init();
    fb_init();
    printf("Frame Buffer initiated\n");

    print_to_screen("UART initiated...\nHello My OS!\n");
    print_to_screen("IRQ initiated\n");
    print_to_screen("Frame Buffer initiated\n");
    // drawString(100, 100, "UART initiated...\nHello My OS!\n", 0x0f, 2);
    // while(1);


#if RPI_VERSION == 4
    printf("\tRunning on Rpi 4\n");
    print_to_screen("\tRunning on Rpi 4\n");
#endif

#if INIT_MMU == 1
    printf("Initialized MMU\n");
    print_to_screen("Initialized MMU\n");
#endif

    printf("ALIGN CHECK\n");
    print_to_screen("ALIGN CHECK\n");
    struct align_check1 ac1;
    memcpy(&ac1, buffer, 7);
    printf("UNPACKED: A: %X, B: %X, C: %X, D: %X\n", ac1.a, ac1.b, ac1.c, ac1.d);
    // print_to_screen(my_sprintf("UNPACKED: A: %X, B: %X, C: %X, D: %X\n", ac1.a, ac1.b, ac1.c, ac1.d));
    struct align_check2 ac2;
    memcpy(&ac2, buffer, 7);
    printf("PACKED: A: %X, B: %X, C: %X, D: %X\n", ac2.a, ac2.b, ac2.c, ac2.d);
    // print_to_screen(my_sprintf("PACKED: A: %X, B: %X, C: %X, D: %X\n", ac2.a, ac2.b, ac2.c, ac2.d));
    void *p1 = get_free_pages(10);
    void *p2 = get_free_pages(4);
    void *p3 = allocate_memory(20 * 4096 + 1);
    free_memory(p1);
    free_memory(p2);
    free_memory(p3);

    printf("\nKernel Exception Level %d\n", get_el());
    printf("\nKernel Initialization Done\n");
    printf("\nWelcome to Hanmin's 'Operating System'\n");
    printf("\nType something and you shall see response.\n");
    // print_to_screen(my_sprintf("\nKernel Exception Level %d\n\nKernel Initialization Done\n\nWelcome to Hanmin's 'Operating System'\n\nType something and you shall see response.\n", get_el()));

    printf("Sleeping for 200 ms...\n");
    print_to_screen("Sleeping for 200 ms...\n");
    timer_sleep(200);
    printf("Sleeping for 2000 ms...\n");
    print_to_screen("Sleeping for 2000 ms...\n");
    timer_sleep(2000);
    printf("Sleeping test done.\n");
    print_to_screen("Sleeping test done.\n");

    printf("Game Initializing\n");
    print_to_screen("Game Initializing\n");
    breakout();
}