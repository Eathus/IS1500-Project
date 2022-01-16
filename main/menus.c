#include "menus.h"

#define CHAR_ROW_SIZE 16
#define NUM_ROWS 4
typedef enum button_type{Letter, Number, Enter, Caps, Back, Space} button_type; 
typedef struct Keyboard_button {
    Point pos;
    button_type type;
    char *text;
} Keyboard_button;

int abs(int val){
    return val < 0 ? val * (-1) : val;
}

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

void update_keyboard(uint8_t index, Keyboard_button const *keyboard, uint8_t len){
    int i;
    switch(keyboard[index].type){
        case Letter: case Number:
            invert_char(keyboard[index].pos);
            break;
        case Space: case Back: case Enter: case Caps:
            for(i = 0; i < 3; ++i)
                invert_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y});
            break;
    }
}
char get_caps(button_type type, char ch){
    switch (type)
    {
    case Letter:
        return ch - 32;
    case Number:
        return ch + 5;
    }
}
int mod_sub(int left, int right, int len){
    int res = left - right;
    if(res < 0) res = len - (abs(res) % len);
    return res;
}
void draw_kb_button(uint8_t index, Keyboard_button const *keyboard, uint8_t len, uint8_t caps){
    int i;
    char ch;
    switch(keyboard[index].type){
        case Letter:
            if(caps){}
            write_char(keyboard[index].pos, caps ? keyboard[index].text[0] - 32 : keyboard[index].text[0]);
            break;
        case Number:
            write_char(keyboard[index].pos, caps ? keyboard[index].text[0] + 5 : keyboard[index].text[0]);
            break;
        case Space: case Back: case Enter: case Caps:
            for(i = 0; i < 3; ++i)
                write_char((Point){keyboard[index].pos.x + i - 1, keyboard[index].pos.y}, keyboard[index].text[i]);
            break;
    }
}
void draw_keyboard(Keyboard_button const *keyboard, uint8_t len, uint8_t caps){
    int i;
    for(i = 0; i < len; ++i){
        draw_kb_button(i, keyboard, len, caps);
    }
}
void draw_options(const char * const options[], uint8_t start, uint8_t len){
    int i;
    char empty = '\n';
    if(len < 4)
        for(i = 0; i < 4; ++i){
            if(len <= i) write_string(i, &empty);
            else write_string(i, options[(start + i)]);
        }
    else{
        for(i = start; i < start + 4; ++i)
            write_string(mod_sub(i, start, len), options[i % len]);
    }
}
void options_button_scroll(direction dir, uint8_t *index, uint8_t *start, const char * const options[], uint8_t len) {
    uint8_t prev_page = 0;
    switch(dir) {
        case Up:
            if(*index == *start) prev_page = 1;
            *index = mod_sub(*index, 1, len);
            if(prev_page){
                prev_page = 0;
                *start = mod_sub(*start, 4, len);
                draw_options(options, *start, len);
            }
            break;
        case Down:
            *index = (*index + 1) % len;
            if(*index == (*start + 4) % len){
                *start = mod_sub(*index, 3, len);
                draw_options(options, *start, len);
            }
            break;
    }
}

void options_page_scroll(direction dir, uint8_t *index, uint8_t *start, const char * const options[], uint8_t len) {
    uint8_t prev_page = 0;
    switch(dir) {
        case Up:
            if(*index == *start) prev_page = 1;
            *index = mod_sub(*index, 1, len);
            if(prev_page){
                prev_page = 0;
                if(*index == len - 1 && len % 4 != 0){
                    *start = len - (len % 4);
                    draw_options(options, *start, len % 4);
                }
                else{
                    *start = mod_sub(*start, 4, len);
                    draw_options(options, *start, len);
                }
            }
            break;
        case Down:
            *index = (*index + 1) % len;
            if(*index == (*start + 4) || *index == 0){
                *start = *index;
                if(len - *index < 4) draw_options(options, *start, len - *index);
                else draw_options(options, *start, len);
            }
            break;
    }
}
void update_options(uint8_t index, uint8_t start, uint8_t len){
    invert_string(mod_sub(index, start, len));
}
game_state return_options(){
    const char * const options[] = {
        "1: CANCEL\n",
        "2: RETRY\n",
        "3: BACK TO MENU\n",
        "4: BING BONG\n",
        "5: GONG GONG\n"
    };
    int btn;
    uint8_t start = 0;
    uint8_t index = 0;
    uint8_t const len = 4;
    int update_counter = 0;

    draw_options(options, start, len);
    update_options(index, start, len);
    while (1){
        int inter_flag = (IFS(0) & 0x100) >> 8;
        btn = getbtns();
        if(inter_flag){
            IFS(0) &= (unsigned int)(~0x100);
            ++update_counter;
        }
        if(update_counter == 10){
            update_counter = 0;
            if (btn & 0xC) {
        
            }
            /* BTN2 - AND with corresponding bit*/
            else if(btn & 0x2) {
                update_options(index, start, len);
                options_page_scroll(Up, &index, &start, options, len);
                update_options(index, start, len);
            }
            /* BTN1 - AND with corresponding bit*/
            else if (btn & 0x1) {
                update_options(index, start, len);
                ooptions_page_scroll(Down, &index, &start, options, len);
                update_options(index, start, len);
            }
            update_disp(); 
        }
    }
}
game_state name_input(char *name){
    char upper[3] = {1, 8, 3};
    const Keyboard_button const keyboard[] = {
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

    draw_keyboard(keyboard, len, caps);
    update_keyboard(index, keyboard, len);
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
                invert_char((Point){cursor_index, 0});
            }
            if (sw){
                if(btn){
                    char ch;
                    switch (keyboard[index].type)
                    {
                    case Letter: 
                        if(cursor_index < 10){
                            if(caps) ch = keyboard[index].text[0] - 32;    
                            else ch = keyboard[index].text[0];
                            name[cursor_index] = ch;
                            write_char((Point){cursor_index, 0}, ch);
                            cursor_index++;
                        }
                        break;
                    case Number:
                        if(cursor_index < 10){
                            if(caps) ch = keyboard[index].text[0] + 5;
                            else ch = keyboard[index].text[0];
                            name[cursor_index] = ch;
                            write_char((Point){cursor_index, 0}, ch);
                            cursor_index++;
                        }
                        break;
                    case Space:
                        if(cursor_index < 10){
                            name[cursor_index] = ' ';
                            write_char((Point){cursor_index, 0}, ' ');
                            cursor_index++;
                        }
                        break;
                    case Back:
                        if(0 <= cursor_index){
                            write_char((Point){cursor_index, 0}, '\0');
                            if(0 < cursor_index) cursor_index--;
                            name[cursor_index] = '\0';
                            write_char((Point){cursor_index, 0}, '\0');
                        }
                        break;
                    case Caps:
                        if(caps){
                            caps = 0;
                            draw_keyboard(keyboard, len, caps);
                            update_keyboard(index, keyboard, len);
                        }
                        else{
                            caps = 1;
                            draw_keyboard(keyboard, len, caps);
                        }
                        break;
                    case Enter:
                        return;
                        break;
                    default:
                        break;
                    }
                }
            }
            else{
                if (btn & 0x8) {
                    update_keyboard(index, keyboard, len);
                    keyboard_move(Left, &index, keyboard, len);
                    update_keyboard(index, keyboard, len);
                }
                /* BTN3 - AND with corresponding bit*/
                else if (btn & 0x4) {
                    update_keyboard(index, keyboard, len);
                    keyboard_move(Right, &index, keyboard, len);
                    update_keyboard(index, keyboard, len);
                }
                /* BTN2 - AND with corresponding bit*/
                else if(btn & 0x2) {
                    update_keyboard(index, keyboard, len);
                    keyboard_move(Up, &index, keyboard, len);
                    update_keyboard(index, keyboard, len);
                }
                /* BTN1 - AND with corresponding bit*/
                else if (btn & 0x1) {
                    update_keyboard(index, keyboard, len);
                    keyboard_move(Down, &index, keyboard, len);
                    update_keyboard(index, keyboard, len);
                }
            }
            update_disp(); 
        }
    }
}