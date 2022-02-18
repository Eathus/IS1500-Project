#pragma once
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

typedef enum io_button_inputs
    {
        BTN1 = 1, BTN2, BTN1_2, BTN3, 
        BTN1_3, BTN2_3, BTN1_2_3, BTN4,
        BTN1_4, BTN2_4, BTN1_2_4, BTN3_4,
        BTN1_3_4, BTN2_3_4, BTN1_2_3_4
    } io_button_inputs;

int getsw( void );
/* Same as before, bitshift what we want to the least significant bits and remove all else. */
int getbtns( void );