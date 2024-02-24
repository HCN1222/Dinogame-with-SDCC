/*
 * file: testpreempt.c
 */
#include <8051.h>
#include "preemptive.h"
#include "keylib.h"
#include "lcdlib.h"
#include "game.h"
#include "memory.h"

#define CONCAT(x,y) x##y
#define LABEL(x) CONCAT(x, $)

void GameCtrl(void)
{
    //initialize the game
    //GameInit();
    while (1)
    {
        if( GameStart() )
        {
            SemaphoreWaitBody(full, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                //move the dinosaur
                if( SharedBuffer == '2' || SharedBuffer == '8' )
                {
                    MoveDinosaur(SharedBuffer - '0');
                }
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(empty);
        }
        else if( GameOver() )
        {
            //pressed any key to restart the game
            SemaphoreWaitBody(full, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                if( SharedBuffer != '\0' )
                {
                    GameInit();
                }
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(empty);
        }
        else
        {
            SemaphoreWaitBody(full, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                //the input of setting a game should be '0~9'#
                if( (last_input >= '0' && last_input <= '9') && SharedBuffer == '#')
                {
                    SetGameLevel(last_input - '0');
                }
                else if (SharedBuffer != '\0')
                {
                    last_input = SharedBuffer;
                }
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(empty);
        }
    }
}

void KeypadCtrl(void)
{
    Init_Keypad();
    while (1)
    {
        if(AnyKeyPressed())
        {
            tmp = KeyToChar();
            SemaphoreWaitBody(empty, LABEL(__COUNTER__));
            SemaphoreWaitBody(mutex, LABEL(__COUNTER__));
            EA = 0;
                SharedBuffer = tmp;
            EA = 1;
            SemaphoreSignal(mutex);
            SemaphoreSignal(full);
            while(AnyKeyPressed()){}
        }
    }
}
/* [10 pts for this function]
 * the consumer in this test program gets the next item from
 * the queue and consume it and writes it to the serial port.
 * The Consumer also does not return.
 */
void RenderTask(void)
{
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
            EA = 0;
            //LCD_clearScreen();
            if( GameStart() )
            {
                //display the game
                LCD_IRWrite(FORCE_CURSOR_LINE_1_HEAD);
                //the first column is the dinosaur
                if( GetDinosaurRow() == 0)
                {
                    LCD_write_char( '\1' );
                    j = 1;
                }
                else
                {
                    j = 0;
                }
                
                for(; j<15; j++)
                {
                    if( GetGameBoard(0, j) == 1 )
                    {
                        LCD_write_char( '\2' );
                    }
                    else if (GetGameBoard(0, j) == 0)
                    {
                        LCD_write_char( ' ' );
                    }
                    else
                    {
                        LCD_write_char( '#' );
                    }
                }

                LCD_IRWrite(FORCE_CURSOR_LINE_2_HEAD);
                if( GetDinosaurRow() == 1)
                {
                    LCD_write_char( '\1' );
                    j = 1;
                }
                else
                {
                    j = 0;
                }
                for(; j<15; j++)
                {
                    if( GetGameBoard(1, j) == 1 )
                    {
                        LCD_write_char( '\2' );
                    }
                    else if( GetGameBoard(1, j) == 0 )
                    {
                        LCD_write_char( ' ' );
                    }
                    else
                    {
                        LCD_write_char( '#' );
                    }
                }
            }
            else if( GameOver() )
            {
                //display the game over
                LCD_IRWrite(FORCE_CURSOR_LINE_1_HEAD);
                // LCD_write_string(" Game Over!");
                LCD_write_char('G'); LCD_write_char('a'); LCD_write_char('m'); LCD_write_char('e');
                LCD_write_char(' '); LCD_write_char('O'); LCD_write_char('v'); LCD_write_char('e');
                LCD_write_char('r'); LCD_write_char('!'); LCD_write_char(' '); LCD_write_char(' ');
                LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');

                LCD_IRWrite(FORCE_CURSOR_LINE_2_HEAD);
                //LCD_write_string(" Score: ");
                LCD_write_char('S'); LCD_write_char('c'); LCD_write_char('o'); LCD_write_char('r');
                LCD_write_char('e'); LCD_write_char(':'); LCD_write_char(' '); LCD_write_char( GetScore()/10 + '0' );
                LCD_write_char( GetScore()%10 + '0' ); LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');
                LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');
            }
            else
            {
                //display the setting
                LCD_IRWrite(FORCE_CURSOR_LINE_1_HEAD);
                //LCD_write_string(" Lv: last_input");, length = 16
                LCD_write_char('L'); LCD_write_char('v'); LCD_write_char(':'); LCD_write_char(' ');
                LCD_write_char(last_input); LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');
                LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');
                LCD_write_char(' '); LCD_write_char(' '); LCD_write_char(' ');
                LCD_IRWrite(FORCE_CURSOR_LINE_2_HEAD);
                //LCD_write_string(" Press # to play");
                LCD_write_char('P'); LCD_write_char('r'); LCD_write_char('e'); LCD_write_char('s');
                LCD_write_char('s'); LCD_write_char(' '); LCD_write_char('#'); LCD_write_char(' ');
                LCD_write_char('t'); LCD_write_char('o'); LCD_write_char(' '); LCD_write_char('p');
                LCD_write_char('l'); LCD_write_char('a'); LCD_write_char('y');
            }
        }
        EA = 1;
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

    ThreadCreate(GameCtrl);
    ThreadCreate(KeypadCtrl);
    RenderTask();
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