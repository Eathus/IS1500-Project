#pragma once
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */ 

extern uint8_t SCREEN[128*4];

typedef enum pixel_status{Off, On} pixel_status;

typedef struct Point {
    uint8_t x, y;
} Point;

void clear_screen();

uint8_t get_stripe(Point coordinates);
uint8_t get_pixel(Point coordinates);

void set_stripe(Point coordinates, uint8_t val);
void set_pixel(Point coordinates, uint8_t val);

void write_char(Point chcoord, char ch);
void invert_char(Point chcoord);

void write_string(uint8_t line, char *str);
void invert_string(uint8_t line);

void init_disp(void);
void update_disp();