#include "snakegame.h"

int mytime = 0x5957;
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

  IPC(2) = (0x0 << 2 | 0x0) | (IPC(2) & ~0x1F); 
  IFS(0) &= (unsigned int)(~0x100);
  IEC(0) |= (0x100);
  asm volatile("ei");
  return;
}

/*Point prand(){
    Point ret;
    ret.x = (TMR3 + TAIL_PLAYER->x * HEAD_PLAYER->x) % (128 - SEGMENT_SIZE + 1);
    ret.y = (TMR4 + TAIL_PLAYER->y * HEAD_PLAYER->y) % (32 - SEGMENT_SIZE + 1);
    return ret;
}*/
typedef struct Dir_queue {
  uint8_t size;
  int8_t last;
  direction *queue;
} Dir_queue;

void qpush(direction dir, Dir_queue *queue, uint8_t replace){
  if(queue->last < queue->size - 1) {
    queue->last++;
    queue->queue[queue->last] = dir;
  }
  else if (replace) queue->queue[0] = dir;
}
uint8_t qpop(Dir_queue *queue){
  if(queue->last < 0) return -1; 
  int i;
  direction ret = queue->queue[0];
  for(i = 1; i <= queue->last; ++i){
    queue->queue[i-1] = queue->queue[i];
  }
  queue->last--;
  return ret;
}
/* This function is called repetitively from the main program */
game_state game_loop(Point *tail, Point *head, uint16_t *snakes, Point *food_pos ){
  int update_counter = 0;
  direction queue[SEGMENT_SIZE];
  Dir_queue dir_buffer = {SEGMENT_SIZE, -1, queue};

  uint8_t frame_update = 0;
  toggle_food(On, food_pos);
  uint8_t grow_player = 0;
  uint16_t rand_count = 0;  
  /* Declaring the volatile pointer porte*/
  volatile int* porte = (volatile int*) 0xbf886110;
  /* Initialize as 0 */
  *porte = 0;
  /* Variables to handle input */
  int btn;
  while (1){
    /* Active choice to have delay at the top of the loop for easier track keeping*/
    //delay( 1000 );
    int inter_flag = (IFS(0) & 0x100) >> 8;
    btn = getbtns();
    if(inter_flag){
        IFS(0) &= (unsigned int)(~0x100);
        ++update_counter;
    }   
    if(update_counter == 5){
        update_counter = 0;
        /* Update input */

        /* All of these are if statements since they can all be updated at once- not exclusive.
        BTN4 - AND with corresponding bit */
        if (btn & 0x8) {
          qpush(Left, &dir_buffer, 1);
        } 
        /* BTN3 - AND with corresponding bit*/
        else if (btn & 0x4) {
          qpush(Right, &dir_buffer, 1);
        }
        /* BTN2 - AND with corresponding bit*/
        else if(btn & 0x2) {
          qpush(Up, &dir_buffer, 1);
        }
        /* BTN1 - AND with corresponding bit*/
        else if (btn & 0x1) {
          qpush(Down, &dir_buffer, 1);
        }
        if(frame_update == SEGMENT_SIZE){
          uint8_t dir_change = qpop(&dir_buffer);
          if(dir_change != -1) change_dir(dir_change, head, snakes);
          frame_update = 0;
        }
        //update_disp();
        //tick( &mytime );
        /* Increment the pointer as the count goes */
        *porte+=1;
        snake_state sstate = move_snake(head, tail, food_pos, snakes, &grow_player);
        int i;
        switch (sstate)
        {
          case Full:
            clear_screen();
            for(i = 0; i < 128; ++i) set_pixel((Point){i, 0}, 1);
            update_disp(); 
            return;
            break;
          
          case Dead:
          {
            clear_screen();
            Options_button options[] = {
              //casting to avoid "undefined reference to `memcpy'" - error
              (Options_button){Cancel, "1: CANCEL\n"},
              (Options_button){Game, "2: RETRY\n"},
              (Options_button){Options, "3: BACK TO MENU\n"},
            };
          Options_menu menu = {
              options,
              0, 0, 3
          };
            for(i = 0; i < 128; ++i) set_pixel((Point){i, 31}, 1);
            update_disp(); 
            return;
            break;
          }
          default:
            break;
        }
        update_disp(); 
        frame_update++;
    }
  }
}