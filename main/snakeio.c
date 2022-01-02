#include "snakeio.h"

int getsw( void ) { return (PORTD >> 8) & 0x000f; }

/* Same as before, bitshift what we want to the least significant bits and remove all else. */
int getbtns( void ) { return ((PORTD >> 4) & 0x000e) | ((PORTF >> 1) & 0x1); }


