#include "snake.h"

#define STRIPE_COLS 128
#define STRIPE_UNITS 8
#define UNIT_SIZE 2
#define STRIPE_ROWS 4

Point FOOD_POS = {63, 15};

uint16_t SNAKES[128*4];

Point HEAD_PLAYER[SEGMENT_SIZE];
Point TAIL_PLAYER[SEGMENT_SIZE];

Point HEAD_AI[SEGMENT_SIZE];
Point TAIL_AI[SEGMENT_SIZE];


void insert_ubit(uint8_t pos, uint8_t new_bit, uint16_t * ch){
    new_bit &= 0x01;
    uint16_t mask;
    if(new_bit){
        mask = new_bit << pos;
        *ch |= mask; 
    }
    else{
        mask = ((0xFE | new_bit) << pos) + ~(0xFF << pos);
        *ch &= mask;
    }
}

int unit_to_ustripe(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 32) / STRIPE_UNITS;
} 
int get_ustripe_index(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 4);
}
int get_unit_index(uint8_t y){
    return (y % 32) % STRIPE_UNITS;
}

uint16_t get_ustripe(Point coordinates){
    return SNAKES[get_ustripe_index(coordinates)];
}
uint8_t get_unit(Point coordinates){
    uint16_t stripe = SNAKES[unit_to_ustripe(coordinates)];
    return (stripe >> get_unit_index(coordinates.y) * UNIT_SIZE) & 0x0003;
}

void set_ustripe(Point coordinates, uint16_t val){
    SNAKES[get_ustripe_index(coordinates)] = val;
}
void set_unit(Point coordinates, uint8_t val){
    uint8_t index = get_unit_index(coordinates.y) * UNIT_SIZE;
    insert_bit(index, val, SNAKES + unit_to_ustripe(coordinates));
    insert_bit(index + 1, val >> 1, SNAKES + unit_to_ustripe(coordinates));
}

void move_segment(Point* segment, uint8_t distance){
    int i;
    switch (get_unit(*segment))
    {
    case Right:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (segment[i].x + distance) % STRIPE_COLS;
        break;
    case Left:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (segment[i].x - distance) % STRIPE_COLS;
        break;
    case Up:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (segment[i].y - distance) % STRIPE_COLS;
        break;
    case Down:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (segment[i].y + distance) % STRIPE_COLS;
        break;
    }
}

void toggle_segment(Point *segment, pixel_status stat){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_pixel(segment[i], stat);
}
void update_segment(Point *segment, direction dir){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(segment[i], dir);
}

uint8_t point_equal(const Point *left, const Point *right){
    return left->x == right->x && left->y == right->y;
}
uint8_t eat_check(Point *head){
    int i, j, k;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        for(j = 0; j < SEGMENT_SIZE; ++j){
            Point fpos = {FOOD_POS.x + j, FOOD_POS.y + i};
            for(k = 0; k < SEGMENT_SIZE; ++k)
                if(point_equal(&fpos, head + k)) return 1;
        }
    }   
    return 0;
}

void player_eat(){
    int i, j;
    direction dir = get_unit(*HEAD_PLAYER);
    for(i = 0; i < SEGMENT_SIZE; ++i){
        move_segment(HEAD_PLAYER, 1);
        update_segment(HEAD_PLAYER, dir);
        toggle_segment(HEAD_PLAYER, On);
    }
    
}
void ai_eat(){
    int i, j;
    direction dir = get_unit(*HEAD_AI);
    for(i = 0; i < SEGMENT_SIZE; ++i){
        move_segment(HEAD_AI, 1);
        update_segment(HEAD_AI, dir);
        toggle_segment(HEAD_AI, On);
    }
    
}

uint8_t is_dead(Point *head){ 
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        if(get_pixel(head[i])) return 1;
    }
    return 0;
}

uint8_t move_snake(Point *head, Point *tail){
    int i;
    direction dir = get_unit(*head);
    move_segment(head, 1);
    update_segment(head, dir);
    toggle_segment(tail, Off);
    if(is_game_over(head)) return 1;
    toggle_segment(head, On);
    move_segment(tail, 1);
    return 0;
}

void change_dir(direction dir, Point *head){
    int i;
    switch (get_unit(*head))
    {
    case Right: 
        switch (dir)
        {
        case Up:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].y = head[0].y;
                head[i].x = head[i].x - SEGMENT_SIZE - 1 - i;
                set_unit(head[i], dir);
            }
            break;
        case Down:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].y = head[SEGMENT_SIZE - 1].y;
                head[i].x = head[i].x - SEGMENT_SIZE - 1 - i;
                set_unit(head[i], dir);
            }
            break;
        default:
            break;
        }
        break;
    case Left:
        switch (dir)
        {
        case Up:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].y = head[0].y;
                head[i].x = head[i].x + i;
                set_unit(head[i], dir);
            }
            break;
        case Down:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].y = head[SEGMENT_SIZE - 1].y;
                head[i].x = head[i].x + i;
                set_unit(head[i], dir);
            }
            break;
        default:
            break;
        }
        break;
    case Up: 
        switch (dir)
        {
        case Right:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].x = head[SEGMENT_SIZE - 1].x;
                head[i].y = head[i].y + i;
                set_unit(head[i], dir);
            }
            break;
        case Left:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].x = head[0].x;
                head[i].y = head[i].y + i;
                set_unit(head[i], dir);
            }
            break;
        default:
            break;
        }
        break;
    case Down:
        switch (dir)
        {
        case Right:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].x = head[SEGMENT_SIZE - 1].x;
                head[i].y = head[i].y - SEGMENT_SIZE - 1 - i;
                set_unit(head[i], dir);
            }
            break;
        case Left:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                head[i].x = head[0].x;
                head[i].y = head[i].y - SEGMENT_SIZE - 1 - i;
                set_unit(head[i], dir);
            }
            break;
        default:
            break;
        }
        break;
    }
}   


int mytime = 0x5957;
int counter = 0;
char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void game_init( void )
{
  float periodms = 0.1;
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
  TRISD = TRISD & 0x0fe0;
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
    if(counter == 10){
        counter = 0;
        /* Update input */

        /* All of these are if statements since they can all be updated at once- not exclusive.
        BTN4 - AND with corresponding bit */
        if (btn & 0x4) {
          /* Bitshift to first digit */
          mytime = (sw << 12) | (mytime & 0x0fff);
        }
        /* BTN3 - AND with corresponding bit*/
        if (btn & 0x2) {
          /* Bitshift to second digit */
          mytime = (sw << 8) | (mytime & 0xf0ff);
        }
        /* BTN2 - AND with corresponding bit*/
        if (btn & 0x1) {
          /* Bitshift to third digit */
          mytime = (sw << 4) | (mytime & 0xff0f);
        }
        update_display();
        tick( &mytime );
        /* Increment the pointer as the count goes */
        *porte+=1;
    }
  }
}