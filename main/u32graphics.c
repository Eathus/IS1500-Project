#include "u32graphics.h"


void insert_bit(uint8_t pos, uint8_t new_bit, uint8_t * ch){
    new_bit %= 2;
    uint8_t mask;
    if(new_bit){
        mask = new_bit << pos;
        *ch |= mask; 
    }
    else{
        mask = ((0xFE | new_bit) << pos) + ~(0xFF << pos);
        *ch &= mask;
    }
}

uint8_t get_stripe_index(Point coordinates){
    return ((coordinates.x % STRIPE_COLS)) * STRIPE_ROWS + (coordinates.y % STRIPE_ROWS) / STRIPE_BIT_LEN;
}
uint8_t get_pixel_index(uint8_t y){
    return (y % STRIPE_ROWS) % STRIPE_BIT_LEN;
}
uint8_t get_stripe(Point coordinates, const uint8_t *screen){
    return screen[get_stripe_index(coordinates)];
}

uint8_t get_pixel(Point coordinates, const uint8_t *screen){
    uint8_t stripe = get_stripe(coordinates, screen);
    return (stripe >> get_pixel_index(coordinates.y)) & 0xFE;
}

void set_stripe(Point coordinates, uint8_t val, uint8_t *screen){
    screen[get_stripe_index(coordinates)] = val;
}

void set_pixel(Point coordinates, uint8_t val, uint8_t *screen){
    uint8_t index = get_pixel_index(coordinates.y);
    insert_bit(index, val, screen + get_stripe_index(coordinates));
}


