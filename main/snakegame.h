#pragma once
#include "snakeio.h"
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snakelogic.h"
#include "u32graphics.h"

typedef enum game_state{Start, Options, Level_diff, AI_diff, Score_board, Game, End_options, Name_input, Return_options, Cancel} game_state;
typedef enum difficulty{Easy, Normal, Hard} difficulty;

void game_init( void );

/* This function is called repetitively from the main program */
void game_loop(Point *tail, Point *head, uint16_t *snakes, Point *food_pos);