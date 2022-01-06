#include "snakelogic.h"

#define STRIPE_COLS 128
#define STRIPE_UNITS 8
#define UNIT_SIZE 2
#define STRIPE_ROWS 4
#define ROWS 32

Point FOOD_POS = {63, 15};

uint16_t SNAKES[128*4];

Point HEAD_PLAYER[SEGMENT_SIZE];
Point TAIL_PLAYER[SEGMENT_SIZE];

Point HEAD_AI[SEGMENT_SIZE];
Point TAIL_AI[SEGMENT_SIZE];


void insert_ubit(uint8_t pos, uint8_t new_bit, uint16_t * ch){
    new_bit &= 0x01;
    uint16_t mask;
    if(new_bit){
        mask = new_bit << pos;
        *ch |= mask; 
    }
    else{
        mask = ((0xFE | new_bit) << pos) + ~(0xFF << pos);
        *ch &= mask;
    }
}

int unit_to_ustripe(const Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 32) / STRIPE_UNITS;
} 
int get_ustripe_index(const Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 4);
}
int get_unit_index(uint8_t y){
    return (y % 32) % STRIPE_UNITS;
}

uint16_t get_ustripe(const Point coordinates){
    return SNAKES[get_ustripe_index(coordinates)];
}
uint8_t get_unit(const Point coordinates){
    uint16_t stripe = SNAKES[unit_to_ustripe(coordinates)];
    return (stripe >> get_unit_index(coordinates.y) * UNIT_SIZE) & 0x0003;
}

void set_ustripe(Point coordinates, uint16_t val){
    SNAKES[get_ustripe_index(coordinates)] = val;
}
void set_unit(Point coordinates, uint8_t val){
    uint8_t index = get_unit_index(coordinates.y) * UNIT_SIZE;
    insert_ubit(index, val, SNAKES + unit_to_ustripe(coordinates));
    insert_ubit(index + 1, val >> 1, SNAKES + unit_to_ustripe(coordinates));
}

void move_segment(direction dir, Point* segment, uint8_t distance){
    int i;
    switch (dir)
    {
    case Right:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (segment[i].x + distance) % STRIPE_COLS;
        break;
    case Left:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (segment[i].x - distance) % STRIPE_COLS;
        break;
    case Up:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (segment[i].y - distance) % ROWS;
        break;
    case Down:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (segment[i].y + distance) % ROWS;
        break;
    }
}

void set_segment(Point *left, Point *right){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) left[i] = right[i];
}

void toggle_segment(Point *segment, pixel_status stat){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_pixel(segment[i], stat);
}
void update_segment(Point *segment, direction dir){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(segment[i], dir);
}

uint8_t point_equal(const Point *left, const Point *right){
    return left->x == right->x && left->y == right->y;
}
uint8_t eat_check(Point *head){
    int i, j, k;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        for(j = 0; j < SEGMENT_SIZE; ++j){
            Point fpos = {FOOD_POS.x + j, FOOD_POS.y + i};
            for(k = 0; k < SEGMENT_SIZE; ++k)
                if(point_equal(&fpos, head + k)) return 1;
        }
    }   
    return 0;
}
void toggle_food(pixel_status stat){
    int i, j;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        for(j = 0; j < SEGMENT_SIZE; ++j){
            Point fpos = {FOOD_POS.x + j, FOOD_POS.y + i};
            set_pixel(fpos, stat);
        }
    }   
}

uint8_t update_food(Point coordinate){
    Point original = coordinate;
    uint8_t cur_x = coordinate.x - coordinate.y;
    uint8_t max_x = STRIPE_COLS - SEGMENT_SIZE + 1;
    uint8_t max_y = ROWS - SEGMENT_SIZE + 1;

    int i, j;
    uint8_t invalid_pos = 0;
    while(1){
        for(i = 0; i < SEGMENT_SIZE; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                Point fpos = {coordinate.x + j, coordinate.y + i};
                if(get_pixel(fpos)){
                    invalid_pos = 1;
                    break;
                }
            }
            if(invalid_pos) break;
        }
        if(invalid_pos){
            coordinate.y = (coordinate.y + 1) % max_y;
            if(coordinate.y == 0){
                cur_x = (cur_x + 1) % max_x;
                coordinate.x = cur_x;
            }
            else coordinate.x = (coordinate.x + 1) % max_x;
            invalid_pos = 0;
            if(point_equal(&coordinate, &original)) return 1;
        }
        else{
            FOOD_POS = coordinate;
            return 0;
        }    
    }
}

void eat(Point *head){
    int i, j;
    direction dir = get_unit(*head);
    toggle_food(Off);
    for(i = 1; i < SEGMENT_SIZE; ++i){
        update_segment(head, dir);
        toggle_segment(head, On);
        move_segment(get_unit(*head), head, 1);
    }
    update_segment(head, dir);
    toggle_segment(head, On);
}

uint8_t is_dead(const Point *head){ 
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        if(get_pixel(head[i])) return 1;
    }
    return 0;
}

uint8_t rotate_segment(direction current_dir, direction rotation, Point *segment){
    int i;
    switch (current_dir)
    {
    case Right: 
        switch (rotation)
        {
        case Up:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].y = segment[SEGMENT_SIZE - 1].y;
                segment[i].x = segment[i].x - SEGMENT_SIZE + 1 + i;
            }
            break;
        case Down:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].y = segment[0].y;
                segment[i].x = segment[i].x - SEGMENT_SIZE + 1 + i;
            }
            break;
        default:
            return 1;
        }
        break;
    case Left:
        switch (rotation)
        {
        case Up:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].y = segment[SEGMENT_SIZE - 1].y;
                segment[i].x = segment[i].x + i;
            }
            break;
        case Down:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].y = segment[0].y;
                segment[i].x = segment[i].x + i;
            }
            break;
        default:
            return 1;
        }
        break;
    case Up: 
        switch (rotation)
        {
        case Right:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].x = segment[0].x;
                segment[i].y = segment[i].y + i;
            }
            break;
        case Left:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].x = segment[SEGMENT_SIZE - 1].x;
                segment[i].y = segment[i].y + i;
            }
            break;
        default:
            return 1;
        }
        break;
    case Down:
        switch (rotation)
        {
        case Right:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].x = segment[0].x;
                segment[i].y = segment[i].y - SEGMENT_SIZE + 1 + i;
            }
            break;
        case Left:
            for(i = 0; i < SEGMENT_SIZE; ++i){
                segment[i].x = segment[SEGMENT_SIZE - 1].x;
                segment[i].y = segment[i].y - SEGMENT_SIZE + 1 + i;
            }
            break;
        default:
            return 1;
        }
        break;
    }
    return 0;
}   

void change_dir(direction dir, Point *head){
    int i;
    if(rotate_segment(get_unit(*head), dir, head)) return;  
    for(i = 1; i < SEGMENT_SIZE; ++i){
        update_segment(head, dir);
        move_segment(get_unit(*head), head, 1);
    } 
    update_segment(head, dir);
}   

Point prand(){
    Point ret;
    ret.x = (TMR3 + TAIL_PLAYER->x * HEAD_PLAYER->x) % (STRIPE_COLS - SEGMENT_SIZE + 1);
    ret.y = (TMR4 + TAIL_PLAYER->y * HEAD_PLAYER->y) % (ROWS - SEGMENT_SIZE + 1);
    return ret;
}

snake_state move_snake(Point *head, Point *tail, uint8_t *grow){
    int i;
    direction head_dir = get_unit(*head);
    move_segment(get_unit(*head), head, 1);
    if(eat_check(head)){ 
        *grow += SEGMENT_SIZE;
        toggle_food(Off);
        if(update_food(prand())) return Full;
        //if(update_food((Point){(HEAD_PLAYER[0].x + 5) %128, HEAD_PLAYER[0].y})) return Full;
        toggle_food(On);
    }
    if(*grow){
        *grow = *grow - 1;
        toggle_segment(head, On);
        update_segment(head, head_dir); 
        return 0;
    }
    if(is_dead(head)) return Dead;
    toggle_segment(tail, Off);
    toggle_segment(head, On);
    update_segment(head, head_dir);

    //uint8_t prev_tail_dir = get_unit(*tail);

    Point prev_tail[SEGMENT_SIZE];
    set_segment(prev_tail, tail); 
    
    move_segment(get_unit(*tail), tail, 1);

    for(i = 0; i < SEGMENT_SIZE; ++i){
        uint8_t prev_tail_dir = get_unit(prev_tail[i]);
        uint8_t cur_tail_dir = get_unit(tail[i]);
        if(prev_tail_dir != cur_tail_dir){
            move_segment(prev_tail_dir, tail, SEGMENT_SIZE - 1);
            rotate_segment(prev_tail_dir, cur_tail_dir, tail);
            break;
        }
    }
    /*if(cur_tail_dir != prev_tail_dir){
        move_segment(prev_tail_dir, tail, SEGMENT_SIZE - 1);
        rotate_segment(prev_tail_dir, cur_tail_dir, tail);
    }*/
    return Alive;
}


void spawn_snake(Point *tail, Point *head, uint8_t len, direction dir){
    int i;
    head[0] = tail[0];
    switch (dir)
    {
    case Right: case Left:
        for(i = 1; i < SEGMENT_SIZE; ++i){ 
            head[i] = (Point){head[i - 1].x, head[i - 1].y + 1};
            tail[i] = head[i];
        }
        break;
    case Up: case Down:
        for(i = 1; i < SEGMENT_SIZE; ++i){ 
            head[i] = (Point){head[i - 1].x + 1, head[i - 1].y};
            tail[i] = head[i];
        }
        break;
    }
    for(i = 0; i < len; ++i){
        toggle_segment(head, On);
        update_segment(head, dir);
        move_segment(dir, head, 1);
    }
    toggle_segment(head, On);
    update_segment(head, dir);
}

