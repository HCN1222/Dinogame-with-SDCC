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
__data __at (0x25) char Producer1turn;
__data __at (0x26) char Producer2turn;
__data __at (0x27) char msg2;

#define CONCAT(x,y) x##y
#define LABEL(x) CONCAT(x, $)

void Producer1(void)
{
    /*
     * @@@ [2 pt]
     * initialize producer data structure, and then enter
     * an infinite loop (does not return)
     */
    msg = 'A';
    while (1)
    {
        SemaphoreWaitBody(Producer1turn, LABEL(__COUNTER__));
        SemaphoreWaitBody(empty, LABEL(__COUNTER__));
        SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
        EA = 0;
            buffer = msg;
            msg = (msg == 'Z') ? 'A' : msg + 1;
        EA = 1;
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
        SemaphoreSignal(Producer2turn);
    }
}

void Producer2(void)
{
    /*
     * @@@ [2 pt]
     * initialize producer data structure, and then enter
     * an infinite loop (does not return)
     */
    msg2 = '0';
    while (1)
    {
        SemaphoreWaitBody(Producer2turn, LABEL(__COUNTER__));
        SemaphoreWaitBody(empty, LABEL(__COUNTER__));
        SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
        EA = 0;
            buffer = msg2;
            msg2 = (msg2 == '9') ? '0' : msg2 + 1;
        EA = 1;
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
        SemaphoreSignal(Producer1turn);
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
    SemaphoreCreate(Producer1turn, 1);
    SemaphoreCreate(Producer2turn, 0);

    ThreadCreate(Producer1);
    ThreadCreate(Producer2);
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