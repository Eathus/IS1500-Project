#pragma once
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */ 

const uint8_t STRIPE_COLS = 128;
const uint8_t STRIPE_BIT_LEN = 8;
const uint8_t STRIPE_ROWS = 4;
//matrix for screen in column major; stored as a unsigned char array
const uint8_t SCREEN[STRIPE_COLS * STRIPE_ROWS];

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

typedef struct Point {
    uint8_t x, y;
} Point;

uint8_t get_stripe(Point coordinates, const uint8_t *screen);
uint8_t get_pixel(Point coordinates, const uint8_t *screen);

void set_stripe(Point coordinates, uint8_t val, uint8_t *screen);
void set_pixel(Point coordinates, uint8_t val, uint8_t *screen);