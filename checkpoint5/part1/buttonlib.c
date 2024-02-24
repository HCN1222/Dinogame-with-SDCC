#include <8051.h>
#include "buttonlib.h"
/*
 * button library
 * for accessing the button bank in EdSim51
 * It is relatively easy because the buttons are tied to port P2.
 * When a button is pressed, it is 0; when released, it is 1.
 */
// returns true if any button is pressed. false if no button pressed.
char AnyButtonPressed(void) {
	/* @@@ Your code here. returns true  if any bits of P2 is 0; false otherwise.. */
    return (~P2) & 0xFF;
}
// if one of the buttons is pressed, return the ASCII code for the
// highest number button pressed, while ignoring the second highest.
// but if none of the buttons is pressed, return the null '\0'
// character
char ButtonToChar(void) {
        if ((~P2) & 0x80)
        {
            return '7';
        /* @@@ Your Code here. depending on button state,\
        return '6', '5', '4', … '0', or null if no key is pressed or if the combination is not valid.  */
        }
        else if( (~P2) & 0x40)
        {
            return '6'; 
        }
        else if( (~P2) & 0x20)
        {
            return '5'; 
        }
        else if( (~P2) & 0x10)
        {
            return '4'; 
        }
        else if( (~P2) & 0x08)
        {
            return '3'; 
        }
        else if( (~P2) & 0x04)
        {
            return '2'; 
        }
        else if( (~P2) & 0x02)
        {
            return '1'; 
        }
        else if( (~P2) & 0x01)
        {
            return '0'; 
        }
        else
        {
            return '!';
        }
}