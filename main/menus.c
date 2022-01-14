#include "menus.h"

#define CHAR_ROW_SIZE 16
#define NUM_ROWS 4

typedef struct  Keyboard{
    const Point enter;
    const Point caps;
    const Point back;
    const Point space;
    char const * const keyboard[4];
    char const letters_per_row[4];
} Keyboard;

typedef struct Name_menu{
    uint8_t caps_on;
    Point button_pos;
    Keyboard display;
} Name_menu;

typedef enum direction{Right, Left, Up, Down} direction;

void menu_move(direction dir, Name_menu *men){
    switch (dir)
    {
    case Right:
        break;
    case Left:
        /* code */
        break;
    case Up:
        /* code */
        break;
    case Down:
        /* code */
        break;
    default:
        break;
    }
}

void name_input(void){
    const Keyboard menu_caps = {
        {11, 0}, {12, 1}, {12, 2}, {11, 3},
        {
            "           ENTER",
            "ABCDEFGHIJK CAPS",
            "LMNOPQRSTUV BACK",
            "WXYZ56789  SPACE"
        },
        {0, 10, 10, 9}
    };
    const Keyboard menu_norm = { 
        {11, 0}, {12, 1}, {12, 2}, {11, 3},
        {
            "           ENTER", 
        	"bcdefghijk  CAPS", 
	        "lmnopqrstuv BACK",
	        "wxyz01234 	SPACE"
        },
        {0, 10, 10, 9}
    };
    Name_menu menu = {0, {0, 1}, menu_norm};
    int update_counter = 0;
    uint8_t frame_update = 0;   
    PORTE = 0;
    int btn;

    while(1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        Point curent_chcoord = {0, 1};
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
        } 
        if(update_counter == 5){
            if (btn & 0x8) {
                
            }
            /* BTN3 - AND with corresponding bit*/
            else if (btn & 0x4) {
            
            }
            /* BTN2 - AND with corresponding bit*/
            else if(btn & 0x2) {
            
            }
            /* BTN1 - AND with corresponding bit*/
            else if (btn & 0x1) {
            
            }
        }
    }
}