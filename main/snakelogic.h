#pragma once
#include "u32graphics.h"
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

//width of the snake in pixels
#define SEGMENT_SIZE 2 
//max length of the name of a player for when saving their score
#define MAX_NAME_LEN ( 10 )

//a possible direction in the game for a snake to move
typedef enum direction{Right, Left, Up, Down} direction;

/*
state of the snake. Growing means that it just ate and is still getting
loner on this frame so the tail won't receed. Ate means that it's eating
on this frame and full means that the player has maxed out the size of 
the snake and theres no more space for food to spawn.
*/
typedef enum snake_state{Alive, Ate, Growing, Dead, Full} snake_state;

/*
*	Struct:	Score
*	-------------
*   Score of the player playing the current round
*
*   name: name of the player (maximum 10 characters)
*   score: score of the player
*   disp_string: string to actually display on the scoreboard
*   (a combination of the name and score usually)
*/
typedef struct Score{
    char name[MAX_NAME_LEN + 1];
    int score;
    char disp_stirng[ROW_CHAR_LEN + 1];
} Score;

uint16_t get_ustripe(const Point coordinates, uint16_t const *snakes);
uint8_t get_unit(const Point coordinates, uint16_t const *snakes);

void set_ustripe(Point coordinates, uint16_t, uint16_t *snakes);
void set_unit(Point coordinates, uint8_t val, uint16_t *snakes);

void set_segment(Point *left, const Point *right);

snake_state update_player_snake(Point *head, Point *tail, Point *food_pos, uint8_t *grow, uint16_t *snakes, uint8_t *frame);

snake_state update_ai_snake(Point *head, Point *tail, Point *food_pos, uint8_t *grow, uint16_t *snakes, uint8_t *frame, uint8_t *tracker);

uint8_t eat_check(Point *head, Point const *food_pos);

void eat(Point *head, Point *food_pos, uint16_t *snakes, uint8_t *frame);

void change_dir(direction dir, Point *head, uint16_t *snakes);

void spawn_snake(Point *tail, Point *head, uint8_t len, direction dir, uint16_t *snakes, uint8_t *frame);

void toggle_food(uint8_t *frame, pixel_status stat, Point const *food_pos);

snake_state status_ahead(direction dir, const Point* segment, uint8_t distance, Point food_pos, uint8_t *frame);

uint8_t get_rotated(direction current_dir, direction rotation, const Point *segment, Point* ret);

uint8_t update_food(uint8_t *frame, Point coordinate, Point *food_pos);

int irand(Point *tail, Point *head);
Point prand(Point *tail, Point *head);

direction snake_ai(Point *head, Point food_pos, Point check_dists, uint16_t* snakes, uint8_t *frame);