#pragma once
#include "snakeio.h"
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snakelogic.h"
#include "u32graphics.h"

void game_init( void );

/* This function is called repetitively from the main program */
void game_loop(Point *tail, Point *head, uint16_t *snakes, Point *food_pos);