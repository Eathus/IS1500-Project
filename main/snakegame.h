#pragma once
#include "snakeio.h"
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "snakelogic.h"
#include "u32graphics.h"
#include "menus.h"

typedef enum difficulty{Easy, Normal, Hard} difficulty;

void game_init( void );

/* This function is called repetitively from the main program */
game_state game_loop(Point *tail, Point *head, uint16_t *snakes, Point *food_pos);