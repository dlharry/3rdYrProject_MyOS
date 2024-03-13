#include "utils.h"
#include "printf.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "peripherals/aux.h"
#include "mini_uart.h"
#include "timer.h"
#include "irq.h"

const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void show_invalid_entry_message(u32 type, u64 esr, u64 address) {
    printf("ERROR CAUGHT: %s - %d, ESR: %X, Address: %X\n", 
        entry_error_messages[type], type, esr, address);
}

// To add a new interrupt controller
void enable_interrupt_controller() {
    #if RPI_VERSION == 4
        REGS_IRQ->irq0_enable_0 = AUX_IRQ | SYS_TIMER_IRQ_1 | SYS_TIMER_IRQ_3 ;
    #endif

    #if RPI_VERSION == 3
        REGS_IRQ->irq0_enable_1 = AUX_IRQ;
    #endif
}

u32 is_iir_ready(){
    return (REGS_AUX->mu_iir & 4) == 4;
}

void handle_irq() {
    u32 irq;

    irq = REGS_IRQ->irq0_pending_0;

    while(irq) {
        // uart
        if (irq & AUX_IRQ) {
            irq &= ~AUX_IRQ;

            while(is_iir_ready()) {
                uart_input_char_from_irq = uart_recv();
                printf("UART Recv: %c\n", uart_input_char_from_irq);
                // uart_send(ch);
                // return ch;
            }
        }

        // every 1 s
        if (irq & SYS_TIMER_IRQ_1){
            irq &= ~SYS_TIMER_IRQ_1;
            handle_timer_1();
        }

        // every 1/4 s
        if (irq & SYS_TIMER_IRQ_3){
            irq &= ~SYS_TIMER_IRQ_3;
            handle_timer_3();
        }
        // printf("checking irq\n");
        // return 0;
    }

}