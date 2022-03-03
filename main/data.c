
#include "u32graphics.h"

const uint8_t const FONT[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	255, 129, 129, 129, 129, 129, 129, 129,
	133, 133, 133, 133, 133, 133, 133, 133,
	129, 129, 129, 129, 129, 129, 129, 255,
	24, 24, 60, 36, 102, 66, 195, 129, 	
	129, 195, 231, 189, 153, 189, 231, 195,
	63, 33, 33, 33, 33, 33, 33, 225,
	137, 157, 191, 137, 137, 137, 137, 143,
	145, 153, 253, 135, 135, 253, 153, 145,
	8, 28, 62, 8, 8, 8, 8, 14,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

uint8_t DIFFICULTY_HELP[] = {
	127, 73, 73, 54, 1, 1, 127, 1,
	1, 0, 127, 2, 4, 8, 127, 0, 24, 
	20, 18, 127, 16, 0, 34, 8, 28,
	62, 8, 8, 8, 8, 0, 0, 0, 0,
	127, 73, 73, 54, 1, 1, 127, 1,
	1, 0, 127, 2, 4, 8, 127, 0, 34,
	65, 73, 54, 0, 34, 8, 28, 62, 8,
	8, 8, 14, 0, 0, 0, 0, 127, 73, 73,
	54, 1, 1, 127, 1, 1, 0, 127, 2, 4, 
	8, 127, 0, 98, 81, 73, 69, 66, 0, 
	34, 0, 4, 6, 127, 6, 4, 0, 0, 0, 0, 
	127, 73, 73, 54, 1, 1, 127, 1, 1, 0, 
	127, 2, 4, 8, 127, 0, 66, 127, 64,
	0, 34, 0, 16, 48, 127, 48, 16 
};

uint8_t SCOREBOARD_HELP[] = {
	127, 73, 73, 54, 1, 1, 127, 1,
	1, 0, 127, 2, 4, 8, 127, 0, 24, 
	20, 18, 127, 16, 0, 64, 48, 8,
	6, 1, 0, 34, 65, 73, 54, 0, 34, 8, 28, 62, 8,
	8, 8, 8, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,  127, 
	73, 73, 54, 1, 1, 127, 1, 1, 0, 127,
	2, 4, 8, 127, 0, 98, 81, 73, 69, 66,
	0, 34, 0, 4, 6, 127, 6, 4, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	127, 73, 73, 54, 1, 1, 127, 1, 1, 0, 
	127, 2, 4, 8, 127, 0, 66, 127, 64,
	0, 34, 0, 16, 48, 127, 48, 16 
};

uint8_t LOCATOR_MENU_HELP[] = {
	127, 73, 73, 54, 1, 1, 127, 1,
	1, 0, 127, 2, 4, 8, 127, 0, 24, 
	20, 18, 127, 16, 0, 64, 48, 8,
	6, 1, 0, 34, 65, 73, 54, 0, 34, 8, 28, 62, 8,
	8, 8, 14, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 127, 
	73, 73, 54, 1, 1, 127, 1, 1, 0, 127,
	2, 4, 8, 127, 0, 98, 81, 73, 69, 66,
	0, 34, 0, 4, 6, 127, 6, 4, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	127, 73, 73, 54, 1, 1, 127, 1, 1, 0, 
	127, 2, 4, 8, 127, 0, 66, 127, 64,
	0, 34, 0, 16, 48, 127, 48, 16 
};


uint8_t HANG_OBSTICAL1[] = {
	7, 255, 255, 255, 255,
	0, 15, 15, 15, 3
};

uint8_t HANG_OBSTICAL2[] = {
	248, 255, 255, 255, 255,
	255, 255, 255, 255, 255,
	1, 1, 1, 1, 1
};

uint8_t HANG_OBSTICAL3[] = {
	252, 252, 248, 248
};

uint8_t HANG_OBSTICAL4[] = {
	0, 255, 255, 255, 255, 255, 255,
	248, 255, 255, 255, 255, 31, 3,
	15, 15, 31, 15, 1, 0, 0
};

uint8_t HANG_OBSTICAL5[] = {
	255, 255, 127, 127
};

uint8_t START_SCREEN[] = {
	240, 248, 156, 14, 6, 6, 6, 6, 14, 28, 
	56, 48, 48, 240, 240, 0, 192, 192, 254, 
	254, 6, 6, 6, 254, 254, 56, 112, 224, 
	192, 128, 0, 0, 0, 254, 254, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 224, 240, 56,
	28, 14, 6, 6, 14, 28, 56, 240, 224, 0,
	240, 240, 48, 240, 240, 128, 254, 254,
	6, 6, 254, 254, 0, 0, 128, 192, 224, 112,
	56, 28, 14, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 254, 254, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 240, 248, 56, 200, 136, 184, 136,
	208, 32, 32,

	0, 1, 3, 7, 6, 6, 6, 6, 14, 28, 248,
	240, 0, 15, 15, 12, 15, 15, 0, 0, 0, 0, 0,
	255, 255, 0, 0, 0, 1, 3, 7, 14, 28, 255, 255,
	0, 0, 248, 248, 24, 248, 248, 0, 255, 255, 3,
	255, 255, 3, 3, 3, 3, 3, 3, 3, 3, 255, 255, 3,
	3, 3, 0, 1, 1, 1, 1, 1, 0, 0, 255, 255, 6, 15, 31,
	57, 112, 224, 192, 128, 0, 0, 0, 240, 240, 48,
	240, 240, 0, 0, 0, 0, 255, 255, 6, 6, 6, 6, 6, 
	0, 0, 0, 0, 0, 0, 0, 0, 240, 240, 48, 240, 240,
	0, 255, 255, 3, 255, 255, 0, 255, 255, 0, 0, 0,
	0, 0, 0, 0, 0,
	
	7, 14, 28, 24, 24, 24, 24, 24, 28, 14, 7, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 0, 0, 0,
	0, 0, 0, 0, 0, 31, 31, 24, 24, 31, 31, 0, 7, 7,
	6, 7, 7, 0, 31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 31,
	31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 0,
	0, 0, 0, 0, 0, 1, 3, 31, 30, 24, 31, 31, 0, 3, 
	3, 31, 31, 24, 24, 31, 31, 24, 24, 24, 24, 24,
	24, 24, 31, 31, 3, 31, 31, 24, 31, 31, 0, 31,
	31, 24, 31, 31, 0, 31, 31, 24, 31, 31, 0, 0, 0,
	0, 0, 0, 0, 0,

	0, 0, 124, 130, 130, 130, 68, 0, 254, 128, 128, 128,
	0, 130, 254, 130, 0, 124, 130, 130, 130, 68, 0, 254, 
	16, 40, 198, 0, 0, 0, 252, 10, 10, 10, 252, 0, 
	254, 4, 8, 16, 254, 0, 2, 4, 248, 4, 2, 0, 0, 
	0, 254, 146, 146, 108, 2, 2, 254, 2, 2, 0, 254, 
	4, 8, 16, 254, 0, 0, 0, 2, 2, 254, 2, 2, 0, 124,
	130, 130, 124, 0, 0, 0, 124, 130, 130, 130, 68, 0, 124, 
	130, 130, 124, 0, 254, 4, 8, 16, 254, 0, 2, 2, 254, 2,
	2, 0, 130, 254, 130, 0, 254, 4, 8, 16, 254, 0, 126,
	128, 128, 126, 0, 254, 146, 146, 130, 0, 0
};