#include "snake.h"

#define STRIPE_COLS 128
#define STRIPE_UNITS 8
#define UNIT_SIZE 2
#define STRIPE_ROWS 4

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

void move_segment(Point* segment){
    int i;
    switch (get_unit(*segment))
    {
    case Right:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x++;
        break;
    case Left:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x--;
        break;
    case Up:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y--;
        break;
    case Down:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y++;
        break;
    }
}

void toggle_segment(Point *segment, pixel_status stat){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_pixel(segment[i], stat);
}

void move_snake(Point *head, Point *tail){
    int i;
    direction dir = get_unit(*head);
    move_segment(head);
    for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(head[i], dir);
    toggle_segment(tail, Off);
    toggle_segment(head, On);
    move_segment(tail);
}

void move_snakes(){
    move_snake(HEAD_PLAYER, TAIL_PLAYER);
    move_snake(HEAD_AI, TAIL_AI);
}

void change_dir(direction dir, Point *head){
    int i;
    switch (get_unit(*head))
    {
    case Right: case Left:
        switch (dir)
        {
        case Up: case Down:
            for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(head[i], dir);
            break;
        default:
            break;
        }
        break;
    case Up: case Down:
        switch (dir)
        {
        case Right: case Left:
            for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(head[i], dir);
            break;
        default:
            break;
        }
        break;
    }
}