#pragma once
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */ 

extern uint8_t SCREEN[128*4];
#define ROW_CHAR_LEN ( 16 )

typedef enum pixel_status{Off, On} pixel_status;

typedef struct Point {
    uint8_t x, y;
} Point;

typedef struct Image{
	Point pos; //where the top left pixel is
	uint8_t width;
	uint8_t height;
	uint8_t *image; 
} Image;

void clear_screen();

uint8_t get_stripe(Point coordinates);
uint8_t get_pixel(Point coordinates);

void set_stripe(Point coordinates, uint8_t val);
void set_pixel(Point coordinates, uint8_t val);

void write_char(Point chcoord, char ch);
void invert_char(Point chcoord);

void write_row(uint8_t line, char const *str);
void invert_row(uint8_t line);

void write_string(uint8_t line, char const *str, uint8_t len);
void invert_string(uint8_t line, uint8_t len);

void update_disp();


//Copied from Lab 3
void init_disp(void);
void num32asc( char * s, int n );
char * itoaconv( int num, int *len);