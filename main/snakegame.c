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
  T2CON = 0x0070;
  T2CONSET = 0x8000;
  
  T3CONCLR = 0x8000;
  T3CON = 0x0;
  T3CONSET = 0x8000;

  T4CONCLR = 0x8000;
  T4CON = 0x40;
  T4CONSET = 0x8000;
  
  PR2 = period;
  TMR2 = 0;
  TMR3 = 0;
  TMR4 = 0;
  /* Modifying the 8 least significant bits set as output*/
  TRISE &= 0xff00;
  /* Initialize port D with the definitions in the pic32mx.h file. Bits
  11 through 5 are set as input. */
  TRISD |= 0x0fe0;
  TRISF |= 0x0002;
  return;
}

/*Point prand(){
    Point ret;
    ret.x = (TMR3 + TAIL_PLAYER->x * HEAD_PLAYER->x) % (128 - SEGMENT_SIZE + 1);
    ret.y = (TMR4 + TAIL_PLAYER->y * HEAD_PLAYER->y) % (32 - SEGMENT_SIZE + 1);
    return ret;
}*/

/* This function is called repetitively from the main program */
void game_loop( void )
{
  toggle_food(On);
  uint8_t grow_player = 0;
  uint16_t rand_count = 0;  
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
    if(counter == 7 ){
        counter = 0;
        /* Update input */

        /* All of these are if statements since they can all be updated at once- not exclusive.
        BTN4 - AND with corresponding bit */
        if (btn & 0x8) {
            change_dir(Left, HEAD_PLAYER);
        }
        /* BTN3 - AND with corresponding bit*/
        else if (btn & 0x4) {
          /* Bitshift to second digit */
          change_dir(Right, HEAD_PLAYER);
        }
        /* BTN2 - AND with corresponding bit*/
        else if(btn & 0x2) {
          /* Bitshift to third digit */
          change_dir(Up, HEAD_PLAYER);
        }
        /* BTN1 - AND with corresponding bit*/
        else if (btn & 0x1) {
          /* Bitshift to third digit */
          change_dir(Down, HEAD_PLAYER);
        }
        //update_disp();
        //tick( &mytime );
        /* Increment the pointer as the count goes */
        *porte+=1;
        if(move_snake(HEAD_PLAYER, TAIL_PLAYER, &grow_player)) break;
        update_disp(); 
    }
  }
}