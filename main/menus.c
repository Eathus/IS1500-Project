#include "menus.h"
#include "string.h"
#include "stdio.h"

//how many characters fit in a row on the screen
#define CHAR_ROW_SIZE 16
//number of rows on the screen
#define NUM_ROWS 4

/*
*	Function:	abs
*	---------------
*	Calculate the absolute value of an integer
*
*   val: value to calculate absolute value of
*
*	returns: absolute value
*/
int abs(int val){
    return val < 0 ? val * (-1) : val;
}

/*
*	Function:	keyboard_move
*	-------------------------
*	move cursor in keyboard menu
*
*	dir: 2-dimensional direction to move in the menu
* 	index: current index pointing to the butten currently
*   selected through the "keyboard" array
*   keyboard: array of all the buttons on the keyboard
*	len: length of the keyboard array
*
*	returns: void
*/
void keyboard_move(direction dir, uint8_t *index, Keyboard_button const *keyboard, uint8_t len){
    int i = 0;
    uint8_t current_y = keyboard[*index].pos.y;
    uint8_t temp_index = *index;
    Point current = keyboard[*index].pos;
    switch (dir)
    {
    case Right:
        *index = (*index + 1) % len;
        break;
    case Left:
        if(*index == 0) *index = len - 1;
        else *index-=1;
        break;
    case Up:
        while(1){
            if(current.y != keyboard[*index].pos.y){
                if(abs(keyboard[*index].pos.x - current.x) <= abs((int)keyboard[*index - 1].pos.x - (int)current.x))
                    return;
            }
            if(temp_index == 0) temp_index = len - 1;
            else temp_index -= 1;
            if(current_y != keyboard[temp_index].pos.y){
                if(++i > 1) return;
                current_y = keyboard[temp_index].pos.y;
            }
            *index = temp_index;
        }
        break;
    case Down:
        while(1){
            if(current.y != keyboard[*index].pos.y){
                if(abs(keyboard[*index].pos.x - current.x) <= abs((int)keyboard[*index + 1].pos.x - (int)current.x))
                    return;
            }
            temp_index = (temp_index + 1) % len;
            if(current_y != keyboard[temp_index].pos.y){
                if(++i > 1) return;
                current_y = keyboard[temp_index].pos.y;
            }
            *index = temp_index;
        }
        break;
        break;
    default:
        break;
    }
}

/*
*	Function:	update_keyboard
*	---------------------------
*	Update keyboard based on current position of the cursor
*   (button the cursor is hovering over)
*
* 	index: current index pointing to the butten currently
*   selected through the "keyboard" array
*   keyboard: array of all the buttons on the keyboard
*	len: length of the keyboard array
*   frame: array representing a screen matrix
*
*	returns: void
*/
void update_keyboard(uint8_t index, Keyboard_button const *keyboard, uint8_t len, uint8_t *frame){
    int i;
    switch(keyboard[index].type){
        case Letter: case Number:
            invert_char(frame, keyboard[index].pos);
            break;
        case Space: case Back: case Enter: case Caps:
            for(i = 0; i < 3; ++i)
                invert_char(frame, (Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y});
            break;
    }
}
/*
*	Function:	get_caps
*	--------------------
*	get uppercase ascii decimal value from a lowercase letter
*   or the next 5 decimals after 4 in order to have a caps lock
*   mode to teh keyboard
*
* 	type: wether we are converting a letter or a decimal
*   ch: ascii decimal value of letter or decimal
*
*	returns: converted ascii character decimal value
*/
char get_caps(button_type type, char ch){
    switch (type)
    {
    case Letter:
        return ch - 32;
    case Number:
        return ch + 5;
    }
}

/*
*	Function:	mod_sub
*	-------------------
*	modulo subtraction function 
*   (when we go lower than zero it loops) helps with movement
*   across menus so that they can loop in different ways
*   
*   
*
* 	left: left hand side of subtraction
* 	right: right hand side of subtraction
*   len: modulo (when to loop)
*
*	returns: result of calculation
*/
int mod_sub(int left, int right, int len){
    int res = left - right;
    int diff = abs(res) % len;
    if(res < 0) res = len - (diff == 0 ? len : diff);
    return res;
}

/*
*	Function:	draw_kb_button
*	--------------------------
*	draw all the keyboard buttons (menu) on the screen matrix
*   
* 	index: current index pointing to the butten currently
*   selected through the "keyboard" array
*   keyboard: array of all the buttons on the keyboard
*	len: length of the keyboard array
*   caps: wether to write caps locked version (1) or
*   normal version (0) of keyboard
*   frame: array representing a screen matrix
*
*	returns: void
*/
void draw_kb_button(uint8_t index, Keyboard_button const *keyboard, uint8_t len, uint8_t caps, uint8_t *frame){
    int i;
    char ch;
    switch(keyboard[index].type){
        case Letter:
            if(caps){}
            write_char(frame, keyboard[index].pos, caps ? keyboard[index].text[0] - 32 : keyboard[index].text[0]);
            break;
        case Number:
            write_char(frame, keyboard[index].pos, caps ? keyboard[index].text[0] + 5 : keyboard[index].text[0]);
            break;
        case Space: case Back: case Enter: case Caps:
            for(i = 0; i < 3; ++i)
                write_char(frame, (Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y}, keyboard[index].text[i]);
            break;
    }
}

/*
*	Function:	draw_keyboard
*	-------------------------
*	draw the entire keyboard meant for saving your name 
*   when saving a score
*   
* 	index: current index pointing to the butten currently
*   selected through the "keyboard" array
*   keyboard: array of all the buttons on the keyboard
*	len: length of the keyboard array
*   caps: wether to write caps locked version (1) or
*   normal version (0) of keyboard
*   frame: array representing a screen matrix
*
*	returns: void
*/
void draw_keyboard(Keyboard_button const *keyboard, uint8_t len, uint8_t caps, uint8_t *frame){
    int i;
    for(i = 0; i < len; ++i){
        draw_kb_button(i, keyboard, len, caps, frame);
    }
}

/*
*	Function:	draw_options
*	------------------------
*	draw a generall options menu 
*   (a scroll through menu with a bunch of different options)
*   
* 	menu: actual options to display aswell ass information
*   about those options through the Options_menu struct
*   selected through the "keyboard" array
*   frame: array representing a screen matrix
*
*	returns: void
*/
void draw_options(Options_menu const *menu, uint8_t *frame){
    int i;
    char empty = '\n';
    for(i = 0; i < menu->page_len; ++i){
        if(menu->len <= i + menu->start) write_row(frame, i + 4 - menu->page_len, &empty);
        else{
            write_row(frame, i + 4 - menu->page_len, menu->options[menu->start + i].text);
            if(menu->options[menu->start + i].invert)
                invert_row(frame, i + 4 - menu->page_len);
        }
    }
}

/*
*	Function:	options_page_scroll
*	-------------------------------
*	controll the scrolling function that allows for more options
*   than the max screen length of a generall options menu 
*   (a scroll through menu with a bunch of different options)
*   
*   dir: wether user is currently moving up or down in on the page scroll
* 	menu: actual options to display aswell ass information
*   about those options through the Options_menu struct
*   selected through the "keyboard" array
*   frame: array representing a screen matrix
*
*	returns: void
*/
void options_page_scroll(direction dir, Options_menu *menu, uint8_t *frame) {
    uint8_t prev_page = 0;
    switch(dir) {
        case Up:
            if(menu->index == menu->start && menu->page_len < menu->len) prev_page = 1;
            menu->index = mod_sub(menu->index, 1, menu->len);
            if(prev_page){
                prev_page = 0;
                if(menu->index == menu->len - 1 && menu->len % menu->page_len != 0) 
                    menu->start = menu->len - (menu->len % menu->page_len);
                else menu->start = mod_sub(menu->start, menu->page_len, menu->len);
                draw_options(menu, frame);
            }
            break;
        case Down:
            menu->index = (menu->index + 1) % menu->len;
            if(menu->index == (menu->start + menu->page_len) || menu->index == 0){
                menu->start = menu->index;
                draw_options(menu, frame);
            }
            break;
    }
}

/*
*	Function:	update_options
*	--------------------------
*	shows current option selected by inverting the associated row
*   of a generall options menu 
*   (a scroll through menu with a bunch of different options)
*   
* 	menu: actual options to display aswell ass information
*   about those options through the Options_menu struct
*   selected through the "keyboard" array
*   frame: array representing a screen matrix
*
*	returns: void
*/
void update_options(Options_menu const * menu, uint8_t *frame){
    invert_row(frame, mod_sub(menu->index, menu->start, menu->len) + 4 - menu->page_len);
    char chars;
}

/*
*	Function:	locator_menu
*	------------------------
*	Main cotroll function for locator menus 
*   (a generall menu primarilly used for locating yourself across the program)
*   
* 	menu: actual options to display aswell ass information
*   about those options through the Options_menu struct
*   selected through the "keyboard" array
*   ret: on witch buttons to return from the menu the currently selected option
*   frame: array representing a screen matrix
*
*	returns: the selected button
*/
uint8_t locator_menu(Options_menu * menu, io_button_inputs ret, uint8_t *frame){
    int btn;
    int update_counter = 0;

    draw_options(menu, frame);
    update_options(menu, frame);
    while (1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        btn = getbtns();
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
        }
        if(update_counter == 10){
            update_counter = 0;
            if (btn & ret) {
                clear_frame(frame);
                return menu->options[menu->index].option;
            }
            else if(btn & 0x2) {
                update_options(menu, frame);
                options_page_scroll(Up, menu, frame);
                update_options(menu, frame);
            }
            else if (btn & 0x1) {
                update_options(menu, frame);
                options_page_scroll(Down, menu, frame);
                update_options(menu, frame);
            }
            else if(btn){
                clear_frame(frame);
                return btn;
            }

            update_disp(frame); 
        }
    }
}

/*
*	Function:	name_input
*	----------------------
*	main controll function for the score saving menu where 
*   user writes there name using a keyboard
*   
* 	name: storage variable for the name being saved by the user
*   frame: array representing a screen matrix
*
*	returns: where to o next in the program after finishing name input
*/
game_state name_input(char *name, uint8_t *frame){
    char upper[3] = {1, 8, 3};
    const Keyboard_button const keyboard[] = {
        //casting to avoid "undefined reference to `memcpy'" - error
        (Keyboard_button){{14, 0}, Enter, "\6\7\3"},
        
        (Keyboard_button){{0, 1}, Letter, "a"}, (Keyboard_button){{1, 1}, Letter, "b"}, (Keyboard_button){{2, 1}, Letter, "c"},
        (Keyboard_button){{3, 1}, Letter, "d"}, (Keyboard_button){{4, 1}, Letter, "e"}, (Keyboard_button){{5, 1}, Letter, "f"},
        (Keyboard_button){{6, 1}, Letter, "g"}, (Keyboard_button){{7, 1}, Letter, "h"}, (Keyboard_button){{8, 1}, Letter, "i"},
        (Keyboard_button){{9, 1}, Letter, "j"}, (Keyboard_button){{10, 1}, Letter, "k"}, (Keyboard_button){{14, 1}, Caps, upper},

        (Keyboard_button){{0, 2}, Letter, "l"}, (Keyboard_button){{1, 2}, Letter, "m"}, (Keyboard_button){{2, 2}, Letter, "n"},
        (Keyboard_button){{3, 2}, Letter, "o"}, (Keyboard_button){{4, 2}, Letter, "p"}, (Keyboard_button){{5, 2}, Letter, "q"},
        (Keyboard_button){{6, 2}, Letter, "r"}, (Keyboard_button){{7, 2}, Letter, "s"}, (Keyboard_button){{8, 2}, Letter, "t"},
        (Keyboard_button){{9, 2}, Letter, "u"}, (Keyboard_button){{10, 2}, Letter, "v"}, (Keyboard_button){{14, 2}, Back, "\4\5\3"},

        (Keyboard_button){{0, 3}, Letter, "w"}, (Keyboard_button){{1, 3}, Letter, "x"}, (Keyboard_button){{2, 3}, Letter, "y"},
        (Keyboard_button){{3, 3}, Letter, "z"}, (Keyboard_button){{4, 3}, Number, "0"}, (Keyboard_button){{5, 3}, Number, "1"},
        (Keyboard_button){{6, 3}, Number, "2"}, (Keyboard_button){{7, 3}, Number, "3"}, (Keyboard_button){{8, 3}, Number, "4"},
        (Keyboard_button){{14, 3}, Space, "\1\2\3"}
    };
    int update_counter = 0;
    int blink_counter = 0;
    uint8_t frame_update = 0;
    uint8_t index = 1;   
    uint8_t const len = 35;
    uint8_t caps = 0;
    uint8_t cursor_index = 0;   
    int btn;
    int sw;

    draw_keyboard(keyboard, len, caps, frame);
    update_keyboard(index, keyboard, len, frame);
    while(1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        btn = getbtns();
        sw = getsw();
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
            ++blink_counter;
        }
        if(update_counter == 10){
            update_counter = 0;
            if(blink_counter >= 70){ 
                blink_counter = 0;
                invert_char(frame, (Point){cursor_index, 0});
            }
            if (sw){
                if(btn){
                    game_state ret;
                    char ch;
                    switch (keyboard[index].type)
                    {
                    case Letter: 
                        if(cursor_index < MAX_NAME_LEN){
                            if(caps) ch = keyboard[index].text[0] - 32;    
                            else ch = keyboard[index].text[0];
                            name[cursor_index] = ch;
                            write_char(frame, (Point){cursor_index, 0}, ch);
                            cursor_index++;
                        }
                        break;
                    case Number:
                        if(cursor_index < MAX_NAME_LEN){
                            if(caps) ch = keyboard[index].text[0] + 5;
                            else ch = keyboard[index].text[0];
                            name[cursor_index] = ch;
                            write_char(frame, (Point){cursor_index, 0}, ch);
                            cursor_index++;
                        }
                        break;
                    case Space:
                        if(cursor_index < MAX_NAME_LEN){
                            name[cursor_index] = ' ';
                            write_char(frame, (Point){cursor_index, 0}, ' ');
                            cursor_index++;
                        }
                        break;
                    case Back:
                        if(0 <= cursor_index){
                            write_char(frame, (Point){cursor_index, 0}, '\0');
                            if(0 < cursor_index) cursor_index--;
                            name[cursor_index] = '\0';
                            write_char(frame, (Point){cursor_index, 0}, '\0');
                        }
                        break;
                    case Caps:
                        if(caps){
                            caps = 0;
                            draw_keyboard(keyboard, len, caps, frame);
                            update_keyboard(index, keyboard, len, frame);
                        }
                        else{
                            caps = 1;
                            draw_keyboard(keyboard, len, caps, frame);
                        }
                        break;
                    case Enter:
                    {
                        Options_button options[] = {
                            //casting to avoid "undefined reference to `memcpy'" - error
                            (Options_button){Cancel, "1: CANCEL\n", 0},
                            (Options_button){Game, "2: RETRY\n", 0},
                            (Options_button){Main, "3: BACK TO MENU\n", 0},
                        };
                        Options_menu menu = {
                            options,
                            0, 0, 3, 3
                        };
                        Image menu_help = {
                            Row, 128, 8, LOCATOR_MENU_HELP
                        };
                        draw_image(SCREEN, &menu_help, (Point){0, 0});
                        ret = locator_menu(&menu, BTN3_4, frame);
                        switch (ret)
                        {
                        case Cancel:
                            draw_keyboard(keyboard, len, caps, frame);
                            update_keyboard(index, keyboard, len, frame);
                            write_string(frame, 0, name, cursor_index);
                            break;
                        default:
                            name[cursor_index] = 0;
                            return ret;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            else{
                if (btn & 0x8) {
                    update_keyboard(index, keyboard, len, frame);
                    keyboard_move(Left, &index, keyboard, len);
                    update_keyboard(index, keyboard, len, frame);
                }
                /* BTN3 - AND with corresponding bit*/
                else if (btn & 0x4) {
                    update_keyboard(index, keyboard, len, frame);
                    keyboard_move(Right, &index, keyboard, len);
                    update_keyboard(index, keyboard, len, frame);
                }
                /* BTN2 - AND with corresponding bit*/
                else if(btn & 0x2) {
                    update_keyboard(index, keyboard, len, frame);
                    keyboard_move(Up, &index, keyboard, len);
                    update_keyboard(index, keyboard, len, frame);
                }
                /* BTN1 - AND with corresponding bit*/
                else if (btn & 0x1) {
                    update_keyboard(index, keyboard, len, frame);
                    keyboard_move(Down, &index, keyboard, len);
                    update_keyboard(index, keyboard, len, frame);
                }
            }
            update_disp(frame); 
        }
    }
}

/*
*	Function:	top_score
*	---------------------
*	Checks wether a score is a top score or not 
*   (should be on top score list)
*   
* 	board: score board
*   score: score to check
*
*	returns: score position if it's a top score
*   otherwise returns -1
*/
int top_score(Sboard const *board, int score) {
  int i;
  for(i = 0; i < board->max_len; i++) 
    if(board->board[i].score <= score || i == board->len) 
        return i;
  
  return -1;
}

/*
*	Function:	assign_string
*	-------------------------
*	copy string into another pointer variable
*   
* 	left: storage variable for copy of string
*   right: string to copy
*   keep_end: wether to keep null character at the end 
*   of string (1) or not (0)
*
*	returns: void
*/
void assign_string(char *left, char const *right, uint8_t keep_end){
    int i;
    for(i = 0; right[i] != 0; ++i){
        left[i] = right[i];
    }
    if(keep_end) left[i] = 0;
}

/*
*	Function:	fill_string
*	-----------------------
*	fill string with a certain characther
*   (usefull forfilling string with space)
*   
* 	ch: what to fill string with
*   right: string to copy
*   len: length of string
*
*	returns: void
*/
void fill_string(char ch, char *str, int len){
    int i;
    for(i = 0; i < len; ++i) str[i] = ch;
}

/*
*	Function:	insert_score
*	------------------------
*	Insert a score into scoreboard from top score to
*   bottom score
*   
* 	board: score board
*   score: score to insert
*
*	returns: void
*/
void insert_score(Sboard *board, Score const *score){
  int i;
  int pos = top_score(board, score->score);
  if(pos == -1) return;
  if(board->len < board->max_len) board->len += 1;
  for(i = board->len - 1; i > pos; --i){
    board->board[i].score = board->board[i - 1].score;    
    assign_string(board->board[i].name, board->board[i - 1].name, 1);
    assign_string(board->board[i].disp_stirng, board->board[i - 1].disp_stirng, 1);
  }

  board->board[pos].score = score->score;
  assign_string(board->board[pos].name, score->name, 1);
  fill_string(' ', board->board[pos].disp_stirng, CHAR_ROW_SIZE);
  assign_string(board->board[pos].disp_stirng, score->name, 0);
  int num_len;
  char *number = itoaconv(score->score, &num_len);
  assign_string(board->board[pos].disp_stirng + (CHAR_ROW_SIZE - num_len), number, 1);
}

/*
*	Function:	score_board_menu
*	----------------------------
*	main function for score board menu
*   where you can check all the top scores
*   
* 	board: score board
*   frame: array representing a screen matrix
*
*	returns: where to go in the program after 
*   exiting the score board
*/
game_state score_board_menu(Sboard const *board, uint8_t *frame){
    if(board->len == 0){
        clear_frame(frame);
        write_row(frame, 0, "NO SAVED SCORES");
        write_row(frame, 2, "CLICK ANY BUTTON");
        write_row(frame, 3, "TO CONTINUE");
        update_disp(frame);
        
        int btn;
        int update_counter = 0;
        while (1){
            int inter_flag = (IFS(0) & 0x100) >> 8;
            btn = getbtns();
            if(inter_flag){
                IFS(0) &= (unsigned int)(~0x100);
                ++update_counter;
            }
            if(update_counter == 10){
                update_counter = 0;
                if(btn) {
                    clear_frame(frame);
                    return Main;
                }
            }
        }
    }
    int i;
    Options_button scores[board->len];
    for(i = 0; i < board->len; ++i){
        scores[i] = (Options_button){Main, board->board[i].disp_stirng, 0};
    }
    Options_menu menu = {
        scores,
        0, 0, board->len, 3
    };
    Image scoreboard_help = {
        Row, 128, 8, SCOREBOARD_HELP
    };
    draw_image(SCREEN, &scoreboard_help, (Point){0, 0});
    return locator_menu(&menu, BTN3_4, frame);
}

/*
*	Function:	diff_menu
*	---------------------
*	main function for difficulty  menu
*   where you adjust a certain difficulty setting
*   for the game
*   
*   diff: variable for string current difficulty selected
* 	menu: actual options to display aswell ass information
*   frame: array representing a screen matrix
*
*	returns: where to go in the program after 
*   exiting the difficulty menu
*/
game_state diff_menu(difficulty * diff, Options_menu * menu, uint8_t *frame){
    Image difficulty_help = {
        Row, 128, 8, DIFFICULTY_HELP
    };
    int current;
    for(current = 0; current < menu->len; ++current){
        if(menu->options[current].option == *diff){
            menu->options[current].text[15] = 9;
            break;    
        }
    }
    while (1)
    {
        draw_image(frame, &difficulty_help, (Point){0, 0});
        difficulty new_diff = locator_menu(menu, BTN3, frame);
        if(new_diff == BTN4) return Main;
        if(new_diff != *diff){
            menu->options[current].text[15] = ' ';
            current = menu->index;
            *diff = new_diff;
            menu->options[menu->index].text[15] = 9;
        }
    }
}

/*
*	Function:	start_screen
*	------------------------
*	Main cotroll function for start screen of the program 
*   
*	returns: void
*/
void start_screen(){
    int btn;
    int update_counter = 0;
    Image menu_help = {
        Row, 128, 32, START_SCREEN
    };
    draw_image(SCREEN, &menu_help, (Point){0, 0});
    update_disp(SCREEN);

    while (1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        btn = getbtns();
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
        }
        if(update_counter == 10){
            update_counter = 0;
            if(btn){
                clear_frame(SCREEN);
                update_disp(SCREEN);
                return;
            }
        }
    }
}