#pragma once
#include "u32graphics.h"
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#define SEGMENT_SIZE 2 

extern uint16_t SNAKES[128*4];

extern Point HEAD_PLAYER[SEGMENT_SIZE];
extern Point TAIL_PLAYER[SEGMENT_SIZE];

extern Point HEAD_AI[SEGMENT_SIZE];
extern Point TAIL_AI[SEGMENT_SIZE];

extern Point FOOD_POS;

typedef enum direction{Right, Left, Up, Down} direction;

uint16_t get_ustripe(const Point coordinates);
uint8_t get_unit(const Point coordinates);

void set_ustripe(Point coordinates, uint16_t);
void set_unit(Point coordinates, uint8_t val);

uint8_t move_snake(Point *head, Point *tail, uint8_t *grow);

uint8_t eat_check(Point *head);

void eat(Point *head);

void change_dir(direction dir, Point *head);

void spawn_snake(Point *tail, Point *head, uint8_t len, direction dir);

void toggle_food(pixel_status stat);