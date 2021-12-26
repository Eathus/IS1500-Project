#pragma once
#include "u32graphics.h"
#include <stdint.h>   /* Declarations of uint_32 and the like */

extern uint16_t SNAKES[128*4];

extern Point HEAD_PLAYER[2];
extern Point TAIL_PLAYER[2];

extern Point HEAD_AI[2];
extern Point TAIL_AI[2];

uint16_t get_ustripe(Point coordinates);
uint8_t get_unit(Point coordinates);

void set_ustripe(Point coordinates, uint16_t);
void set_unit(Point coordinates, uint8_t val);