/*
D1.10.2
When the PE takes an exception to an Exception level that is using AArch64, execution is forced to an address that
is the exception vector for the exception. The exception vector exists in a vector table at the Exception level the
exception is taken to.
A vector table occupies a number of word-aligned addresses in memory, starting at the vector base address.
Each Exception level has an associated Vector Base Address Register (VBAR), that defines the exception base
address for the table at that Exception level.
For exceptions taken to AArch64 state, the vector table provides the following information:
ARM DDI 0487F.c
ID072120
•Whether the exception is one of the following:
—
Synchronous exception.
—
SError.
—
IRQ.
—
FIQ.
*/
#pragma once

#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    8 
#define IRQ_INVALID_EL0_64	    9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64	11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32	15 

//stack frame size
#define S_FRAME_SIZE			256
