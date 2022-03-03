#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "data.h"
#include "u32graphics.h"
#include "snakelogic.h"
#include "snakeio.h"

//difficulty of a aspect of the snake game such as the AI or obsticals
typedef enum difficulty{None, Easy, Normal, Hard} difficulty;

//state of the game (witch part of the game user is currently in)
typedef enum game_state{Quit, Start, Main, Leveld, AId, Score_board, 
Game, End_options, Name_input, Return_options, Cancel} game_state;

//Type of the buttons in a menu deciding what they will do when selected
typedef enum button_type{Letter, Number, Enter, Caps, Back, Space} button_type; 

/*
*	Struct:	Keyboard_button
*	-----------------------
*	A keyboard button for name input 
*	
*	pos:  position on the screen
*	type type of button
*	text: button look through (8x8) characters on the screen
*/
typedef struct Keyboard_button {
    Point const pos;
    button_type const type;
    char const *text;
} Keyboard_button;

/*
*	Struct:	Options_button
*	----------------------
*	An option button relevant to most menus 
*	
*	option:  the option the button presents deciding what it does
*	text: button look through (8x8) characters on the screen
*   invert: wether the button is inverted on the screen (selected) or not 
*/
typedef struct Options_button {
    uint8_t option;
    char *text;
    uint8_t invert;
} Options_button;

/*
*	Struct:	Options_menu
*	--------------------
*	generalised options menu (most menus are some version of this)
*	
*	option: the different buttons in the options menu
*	start: witch option to write first on the screen 
*   (changes depending on scroll postion and relevant if we have more options
*   than rows on the screen)
*   index: current option selected
*   len: number of options in the menu (length of menu)
*   page_len: how many options to show on a singel screen screen before
*   user has to scroll to the next screen (makes it possible to have
*   screen titles)
*/
typedef struct Options_menu {
    Options_button * options;
    uint8_t start;
    uint8_t index;
    uint8_t const len; 
    uint8_t const page_len;
} Options_menu;

/*
*	Struct:	Sboard
*	--------------
*	A struct encapsulating relevant data about the score board 
*	
*	board: Scores currently in the score board
*	len: current length (number of scores) of the score board
*   invert: max length (number of scores) of the score board
*/
typedef struct Sboard{
    Score *board;
    uint8_t len;
    uint8_t max_len;
} Sboard;


game_state name_input(char *name, uint8_t *frame);
uint8_t locator_menu(Options_menu * menu, io_button_inputs ret, uint8_t *frame);
game_state score_board_menu(Sboard const *board, uint8_t *frame);
game_state diff_menu(difficulty * diff, Options_menu * menu, uint8_t *frame);
int top_score(Sboard const *board, int score);
void insert_score(Sboard *board, Score const *score);
void start_screen();