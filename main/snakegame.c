#include "snakegame.h"

int mytime = 0x5957;
int counter = 0;
char textstring[] = "text, more text, and even more text!";

void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void game_init( void )
{
  float periodms = 0.01;
  int scale = 256;
  int ck_freq = 80000000;

  int period = ((periodms * ck_freq) / scale);

  T2CONCLR = 0x8000;
  T2CON = 0x70;
  T2CONSET = 0x8000;
  
  PR2 = period;
  TMR2 = 0;
  /* Initialize Port E to that the 8 least significant bits are set
  as output. Begin by declaring the volatile pointer trise. */
  volatile int* trise = (volatile int*) 0xbf886100;
  /* Modifying the 8 least significant bits*/
  trise = *trise & 0xff00;
  /* Initialize port D with the definitions in the pic32mx.h file. Bits
  11 through 5 are set as input. */
  TRISD = TRISD | 0x0fe0;
  TRISF = TRISF | 0x0002;
  return;
}

/* This function is called repetitively from the main program */
void game_loop( void )
{
  /* Declaring the volatile pointer porte*/
  volatile int* porte = (volatile int*) 0xbf886110;
  /* Initialize as 0 */
  *porte = 0;
  /* Variables to handle input */
  int btn;
  int sw;
  while (1){
    /* Active choice to have delay at the top of the loop for easier track keeping*/
    //delay( 1000 );
    int inter_flag = (IFS(0) & 0x100) >> 8;
    btn = getbtns();
    sw = getsw();
    if(inter_flag){
        IFS(0) = IFS(0) & (unsigned int)(~0x100);
        ++counter;
    }   
    if(counter == 4){
        counter = 0;
        move_snake(HEAD_PLAYER, TAIL_PLAYER);
        update_disp(); 
        /* Update input */

        /* All of these are if statements since they can all be updated at once- not exclusive.
        BTN4 - AND with corresponding bit */
        if (btn & 0x8) {
          /* Bitshift to first digit */
          mytime = (sw << 12) | (mytime & 0x0fff);
        }
        /* BTN3 - AND with corresponding bit*/
        if (btn & 0x4) {
          /* Bitshift to second digit */
          mytime = (sw << 8) | (mytime & 0xf0ff);
        }
        /* BTN2 - AND with corresponding bit*/
        if (btn & 0x2) {
          /* Bitshift to third digit */
          mytime = (sw << 4) | (mytime & 0xff0f);
        }
        /* BTN1 - AND with corresponding bit*/
        if (btn & 0x1) {
          /* Bitshift to third digit */
          mytime = sw | (mytime & 0xfff0);
        }
        //update_disp();
        //tick( &mytime );
        /* Increment the pointer as the count goes */
        *porte+=1;
    }
  }
}