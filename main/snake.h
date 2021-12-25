#pragma once
#include "u32graphics.h"
#include <stdint.h>   /* Declarations of uint_32 and the like */


uint16_t get_ustripe(Point coordinates, const uint16_t *snake);
uint8_t get_unit(Point coordinates, const uint16_t *snake);

void set_ustripe(Point coordinates, uint16_t val, uint16_t *snake);
void set_unit(Point coordinates, uint8_t val, uint16_t *snake);