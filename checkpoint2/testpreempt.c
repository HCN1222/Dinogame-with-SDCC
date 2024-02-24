/*
 * file: testpreempt.c
 */
#include <8051.h>
#include "preemptive.h"
/*
 * @@@ [2pt]
 * declare your global variables here, for the shared buffer
 * between the producer and consumer.
 * Hint: you may want to manually designate the location for the
 * variable. you can use
 * __data __at (0x30) type var;
 * to declare a variable var of the type
 */

__data __at (0x20) char buffer;
__data __at (0x21) char msg;
__data __at (0x22) char full;


/* [8 pts] for this function
 * the producer in this test program generates one characters at a
 * time from 'A' to 'Z' and starts from 'A' again. The shared buffer
 * must be empty in order for the Producer to write.
 */
void Producer(void)
{
    /*
     * @@@ [2 pt]
     * initialize producer data structure, and then enter
     * an infinite loop (does not return)
     */
    msg = 'A';
    while (1)
    {
        /* @@@ [6 pt]
         * wait for the buffer to be available,
         * and then write the new data into the buffer */
        while (full)
        {
        //     ThreadYield();
        }
        EA = 0;
        buffer = msg;
        full = 1;
        msg = (msg == 'Z') ? 'A' : msg + 1;
        EA = 1;
    }
}
/* [10 pts for this function]
 * the consumer in this test program gets the next item from
 * the queue and consume it and writes it to the serial port.
 * The Consumer also does not return.
 */
void Consumer(void)
{
    /* @@@ [2 pt] initialize Tx for polling */
    EA = 0;
        TMOD |= 0x20; // TMOD is also assigned by the (modified) Bootstrap code to set up the timer interrupt in timer-0 for preemption 
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;
    EA = 1;
    while (1)
    {
        /* @@@ [2 pt] wait for new data from producer
         * @@@ [6 pt] write data to serial port Tx,
         * poll for Tx to finish writing (TI),
         * then clear the flag
         */
        while (!full)
        {
        //     ThreadYield();
        }
        EA = 0;
                SBUF = buffer;
                full = 0;
        EA = 1;
        while (!TI)
        {
        //     ThreadYield();
        }
        TI = 0;
    }
}
/* [5 pts for this function]
 * main() is started by the thread bootstrapper as thread-0.
 * It can create more thread(s) as needed:
 * one thread can act as producer and another as consumer.
 */
void main(void)
{
    /*
     * @@@ [1 pt] initialize globals
     * @@@ [4 pt] set up Producer and Consumer.
     * Because both are infinite loops, there is no loop
     * in this function and no return.
     */
    full = 0;
    ThreadCreate(Producer);
    Consumer();
}
void _sdcc_gsinit_startup(void)
{
    __asm
        ljmp _Bootstrap
    __endasm;
}
void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}
void timer0_ISR(void) __interrupt(1) {
        __asm
            ljmp _myTimer0Handler
        __endasm;
}