#pragma once
#include <math.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */ 


extern uint8_t SCREEN[128*4];
#define ROW_CHAR_LEN ( 16 )

typedef enum pixel_status{Off, On} pixel_status;
typedef enum mat_major{Row, Column} mat_major;
typedef struct Point {
    uint8_t x, y;
} Point;

typedef struct Image{
	mat_major im_major;
	uint8_t width;
	uint8_t height;
	uint8_t *image; 
} Image;

void clear_frame(uint8_t *frame);

uint8_t get_stripe(uint8_t *frame, Point coordinates);
uint8_t get_pixel(uint8_t *frame, Point coordinates);

void set_stripe(uint8_t *frame, Point coordinates, uint8_t val);
void set_pixel(uint8_t *frame, Point coordinates, uint8_t val);

void write_char(uint8_t *frame, Point chcoord, char ch);
void invert_char(uint8_t *frame, Point chcoord);

void write_row(uint8_t *frame, uint8_t line, char const *str);
void invert_row(uint8_t *frame, uint8_t line);

void write_string(uint8_t *frame, uint8_t line, char const *str, uint8_t len);
void invert_string(uint8_t *frame, uint8_t line, uint8_t len);

uint8_t get_image_pixel(Point coords, const Image *image);
void draw_image(uint8_t *frame, const Image *image, Point pos);
void draw_image_rotated(uint8_t *frame, const Image *image, Point pos, uint8_t turns, uint8_t clockwise);
void draw_foreground(uint8_t *frame, const Image *image, Point pos, uint8_t forground_mode);
void draw_foreground_rotated(uint8_t *frame, const Image *image, Point pos, uint8_t forground_mode, uint8_t turns, uint8_t clockwise);

void update_disp(uint8_t *frame);


//Copied from Lab 3
void init_disp(void);
void num32asc( char * s, int n );
char * itoaconv( int num, int *len);