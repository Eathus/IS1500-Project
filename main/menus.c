#include "menus.h"

#define CHAR_ROW_SIZE 16
#define NUM_ROWS 4
typedef enum direction{Right, Left, Up, Down} direction;
typedef enum button_type{Letter, Number, Enter, Caps, Back, Space} button_type; 
typedef struct Button {
    Point pos;
    button_type type;
    char *text;
} Button;

int abs(int val){
    return val < 0 ? val * (-1) : val;
}

void menu_move(direction dir, uint8_t *index, Button const *keyboard, uint8_t len){
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
void update_menu(uint8_t index, Button const *keyboard, uint8_t len){
    int i;
    switch(keyboard[index].type){
        case Letter: case Number:
            invert_char(keyboard[index].pos);
            break;
        case Caps: case Back:
            for(i = 0; i < 4; ++i)
                invert_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y});
            break;
        case Enter: case Space:
            for(i = 0; i < 5; ++i)
                invert_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y});
    }
}
void draw_button(uint8_t index, Button const *keyboard, uint8_t len){
    int i;
    switch(keyboard[index].type){
        case Letter: case Number:
            write_char(keyboard[index].pos, keyboard[index].text[0]);
            break;
        case Caps: case Back:
            for(i = 0; i < 4; ++i)
                write_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y}, keyboard[index].text[i]);
            break;
        case Enter: case Space:
            for(i = 0; i < 5; ++i)
                write_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y}, keyboard[index].text[i]);
    }
}
void draw_menu(Button const *keyboard, uint8_t len){
    int i;
    for(i = 0; i < len; ++i){
        draw_button(i, keyboard, len);
    }
}

void name_input(void){
    const Button const keyboard[] = {
        (Button){{12, 0}, Enter, "ENTER"},
        
        (Button){{0, 1}, Letter, "a"}, (Button){{1, 1}, Letter, "b"}, (Button){{2, 1}, Letter, "c"},
        (Button){{3, 1}, Letter, "d"}, (Button){{4, 1}, Letter, "e"}, (Button){{5, 1}, Letter, "f"},
        (Button){{6, 1}, Letter, "g"}, (Button){{7, 1}, Letter, "h"}, (Button){{8, 1}, Letter, "i"},
        (Button){{9, 1}, Letter, "j"}, (Button){{10, 1}, Letter, "k"}, (Button){{13, 1}, Caps, "CAPS"},

        (Button){{0, 2}, Letter, "l"}, (Button){{1, 2}, Letter, "m"}, (Button){{2, 2}, Letter, "n"},
        (Button){{3, 2}, Letter, "o"}, (Button){{4, 2}, Letter, "p"}, (Button){{5, 2}, Letter, "q"},
        (Button){{6, 2}, Letter, "r"}, (Button){{7, 2}, Letter, "s"}, (Button){{8, 2}, Letter, "t"},
        (Button){{9, 2}, Letter, "u"}, (Button){{10, 2}, Letter, "v"}, (Button){{13, 2}, Back, "BACK"},

        (Button){{0, 3}, Letter, "w"}, (Button){{1, 3}, Letter, "x"}, (Button){{2, 3}, Letter, "y"},
        (Button){{3, 3}, Letter, "z"}, (Button){{4, 3}, Number, "0"}, (Button){{5, 3}, Number, "1"},
        (Button){{6, 3}, Number, "2"}, (Button){{7, 3}, Number, "3"}, (Button){{8, 3}, Number, "4"},
        (Button){{12, 3}, Space, "SPACE"}
    };
    int update_counter = 0;
    uint8_t frame_update = 0;
    uint8_t index = 1;   
    uint8_t const len = 35;
    int btn;

    draw_menu(keyboard, len);
    update_menu(index, keyboard, len);
    while(1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        btn = getbtns();
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
        } 
        if(update_counter == 10){
            update_counter = 0;
            if (btn & 0x8) {
                update_menu(index, keyboard, len);
                menu_move(Left, &index, keyboard, len);
                update_menu(index, keyboard, len);
            }
            /* BTN3 - AND with corresponding bit*/
            else if (btn & 0x4) {
                update_menu(index, keyboard, len);
                menu_move(Right, &index, keyboard, len);
                update_menu(index, keyboard, len);
            }
            /* BTN2 - AND with corresponding bit*/
            else if(btn & 0x2) {
                update_menu(index, keyboard, len);
                menu_move(Up, &index, keyboard, len);
                update_menu(index, keyboard, len);
            }
            /* BTN1 - AND with corresponding bit*/
            else if (btn & 0x1) {
                update_menu(index, keyboard, len);
                menu_move(Down, &index, keyboard, len);
                update_menu(index, keyboard, len);
            }
            update_disp(); 
        }
    }
}