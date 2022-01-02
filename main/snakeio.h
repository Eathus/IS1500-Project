#pragma once
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

int getsw( void );
/* Same as before, bitshift what we want to the least significant bits and remove all else. */
int getbtns( void );