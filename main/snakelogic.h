#pragma once
#include "u32graphics.h"
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#define SEGMENT_SIZE 2 
#define MAX_NAME_LEN ( 10 )

extern Point HEAD_PLAYER[SEGMENT_SIZE];
extern Point TAIL_PLAYER[SEGMENT_SIZE];

extern Point HEAD_AI[SEGMENT_SIZE];
extern Point TAIL_AI[SEGMENT_SIZE];

extern Point FOOD_POS;

typedef enum direction{Right, Left, Up, Down} direction;
typedef enum snake_state{Alive, Ate, Dead, Full} snake_state;

typedef struct Score{
    char name[MAX_NAME_LEN + 1];
    int score;
    char disp_stirng[ROW_CHAR_LEN + 1];
} Score;

uint16_t get_ustripe(const Point coordinates, uint16_t const *snakes);
uint8_t get_unit(const Point coordinates, uint16_t const *snakes);

void set_ustripe(Point coordinates, uint16_t, uint16_t *snakes);
void set_unit(Point coordinates, uint8_t val, uint16_t *snakes);

snake_state update_snake(Point *head, Point *tail, Point *food_pos, uint16_t *snakes, uint8_t *grow);

uint8_t eat_check(Point *head, Point const *food_pos);

void eat(Point *head, Point *food_pos, uint16_t *snakes);

void change_dir(direction dir, Point *head, uint16_t *snakes);

void spawn_snake(Point *tail, Point *head, uint8_t len, direction dir, uint16_t *snakes);

void toggle_food(pixel_status stat, Point const *food_pos);