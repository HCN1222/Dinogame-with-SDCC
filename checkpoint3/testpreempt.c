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
__data __at (0x23) char mutex;
__data __at (0x24) char empty;
#define CONCAT(x,y) x##y
#define LABEL(x) CONCAT(x, $)

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
        SemaphoreWaitBody(empty, LABEL(__COUNTER__));
        SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
        EA = 0;
            buffer = msg;
            full = 1;
            msg = (msg == 'Z') ? 'A' : msg + 1;
        EA = 1;
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
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
        SemaphoreWaitBody(full, LABEL(__COUNTER__));
        SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
        EA = 0;
                SBUF = buffer;
                full = 0;
        EA = 1;
        SemaphoreSignal(mutex);
        SemaphoreSignal(empty);
        while (!TI){}
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
    SemaphoreCreate(full, 0);
    SemaphoreCreate(mutex, 1);
    SemaphoreCreate(empty, 1);
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