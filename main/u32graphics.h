#pragma once
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */ 

//const uint8_t STRIPE_COLS;
//const uint8_t STRIPE_BIT_LEN;
//const uint8_t STRIPE_ROWS;
//matrix for screen in column major; stored as a unsigned char array
//uint8_t SCREEN[128*4];

typedef struct Point {
    uint8_t x, y;
} Point;

uint8_t get_stripe(Point coordinates, const uint8_t *screen);
uint8_t get_pixel(Point coordinates, const uint8_t *screen);

void set_stripe(Point coordinates, uint8_t val, uint8_t *screen);
void set_pixel(Point coordinates, uint8_t val, uint8_t *screen);

void init_disp(void);
void update_disp(uint8_t *screen);