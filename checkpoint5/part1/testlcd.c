/*
 * file: testpreempt.c
 */
#include <8051.h>
#include "preemptive.h"
#include "buttonlib.h"
#include "keylib.h"
#include "lcdlib.h"
/*
 * @@@ [2pt]
 * declare your global variables here, for the shared buffer
 * between the producer and consumer.
 * Hint: you may want to manually designate the location for the
 * variable. you can use
 * __data __at (0x30) type var;
 * to declare a variable var of the type
 */

__data __at (0x20) char SharedBuffer;
__data __at (0x21) char full;
__data __at (0x22) char mutex;
__data __at (0x23) char empty;
__data __at (0x24) char ProducerButtonTurn;
__data __at (0x25) char ProducerKeypadTurn;
__data __at (0x26) char tmp;


#define CONCAT(x,y) x##y
#define LABEL(x) CONCAT(x, $)

void ProducerButton(void)
{
    /*
     * @@@ [2 pt]
     * initialize producer data structure, and then enter
     * an infinite loop (does not return)
     */
    while (1)
    {
        SemaphoreWaitBody(ProducerButtonTurn, LABEL(__COUNTER__));

        if(AnyButtonPressed())
        {
            tmp = ButtonToChar();
            SemaphoreWaitBody(empty, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                SharedBuffer = tmp;
                while(AnyButtonPressed()){}
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(full);
        }

        SemaphoreSignal(ProducerKeypadTurn);
    }
}

void ProducerKeypad(void)
{
    /*
     * @@@ [2 pt]
     * initialize producer data structure, and then enter
     * an infinite loop (does not return)
     */
    Init_Keypad();
    while (1)
    {
        SemaphoreWaitBody(ProducerKeypadTurn, LABEL(__COUNTER__));
        if(AnyKeyPressed())
        {
            tmp = KeyToChar();
            while(AnyKeyPressed()){}
            SemaphoreWaitBody(empty, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                SharedBuffer = tmp;
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(full);
        }
        SemaphoreSignal(ProducerButtonTurn);
    }
}
/* [10 pts for this function]
 * the consumer in this test program gets the next item from
 * the queue and consume it and writes it to the serial port.
 * The Consumer also does not return.
 */
void ConsumerLCD(void)
{
    /* @@@ [2 pt] initialize Tx for polling */
    EA = 0;
        TMOD |= 0x20; // TMOD is also assigned by the (modified) Bootstrap code to set up the timer interrupt in timer-0 for preemption 
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;
    EA = 1;
    LCD_Init();
    while (1)
    {
        if( LCD_ready() )
        {
            SemaphoreWaitBody(full, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                LCD_write_char(SharedBuffer);
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(empty);
        }
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
    SemaphoreCreate(ProducerButtonTurn, 1);
    SemaphoreCreate(ProducerKeypadTurn, 0);

    ThreadCreate(ProducerButton);
    ThreadCreate(ProducerKeypad);
    ConsumerLCD();
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