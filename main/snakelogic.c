#include "snakelogic.h"

#define STRIPE_COLS 128
#define STRIPE_UNITS 8
#define UNIT_SIZE 2
#define STRIPE_ROWS 4
#define ROWS 32

/*
*	Function:	insert_ubit
*	-----------------------
*	make any unit bit light up or turn of in a 16-bit unit
*
*	pos: witch bit starting from right most bit
* 	new_bit: turn the bit on (1) or off (0)
*	ch: pointer to variable to insert bit into
*
*	returns: void
*/
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

/*
*	Function:	unit_to_ustripe
*	---------------------------
*	get the index of the 16 bit unit column representing a 8 
*   unit long column on the game field that a specific 
* 	point inhabits
*
*	coordinates:	position of the point
*	
*	returns: the index of the 16-bit unit in the direction array 
*	witch the point inhabits (a unit being a 2-bit value 
*   representing a direction a specific segment of a snake is 
*   moving when inhabiting a specific point)
*/
int unit_to_ustripe(const Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 32) / STRIPE_UNITS;
} 

/*
*	Function:	get_ustripe_index
*	-----------------------------
*	get the index of the 16 bit unit column representing a 8 unit  
*   long column on the game field using ustripe coordinates
*
*	coordinates: coordinate of the ustripe 
*	(0 <= x <= 127, 0 <= y <= 4)
*	
*	returns: the index of the 16-bit unit in the direction array 
*	with the specific ustripe coordinates (a unit being a 2-bit value 
*   representing a direction a specific segment of a snake is 
*   moving when inhabiting a specific point)
*/
int get_ustripe_index(const Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 4);
}

/*
*	Function:	get_unit_index
*	--------------------------
*	get the index of the unit in a specific stripe 
*	(16 bit unit column) representing a unit on the game field
*
*	y: y coordinate of the unit since ustripes are columns 
*	
*	returns: index of the unit relative to the ustripe
*	it inhabits (a unit being a 2-bit value representing 
*   a direction a specific segment of a snake is moving 
*   when inhabiting a specific point)
*/
int get_unit_index(uint8_t y){
    return (y % 32) % STRIPE_UNITS;
}

/*
*	Function:	get_ustripe
*	-----------------------
*	get the value of a ustripe in a unit array 
*	
*	coordinates: coordinates of the stripe	
*   snakes: array representing the unit matrix
*
*	returns: value of a ustripe (16-bit unit column)
*	representing an 8 unit column in a unit matrix
*   (a unit being a 2-bit value representing a direction
*   a specific segment of a snake is moving when inhabiting
*   a specific point)
*/
uint16_t get_ustripe(const Point coordinates, uint16_t const *snakes){
    return snakes[get_ustripe_index(coordinates)];
}

/*
*	Function:	get_unit
*	--------------------
*	get the value of a unit in a unit array 
*	
*	coordinates: coordinates of the unit	
*   snakes: array representing the unit matrix
*
*	returns: value of a unit (a unit being a 2-bit value 
*   representing a direction a specific segment of a snake 
*   is moving when inhabiting a specific point)
*/
uint8_t get_unit(const Point coordinates, uint16_t const *snakes){
    uint16_t stripe = snakes[unit_to_ustripe(coordinates)];
    return (stripe >> get_unit_index(coordinates.y) * UNIT_SIZE) & 0x0003;
}

/*
*	Function:	set_ustripe
*	-----------------------
*	set the value of a ustripe in a unit array 
*   (a unit being a 2-bit value representing a direction
*   a specific segment of a snake is moving when inhabiting
*   a specific point)
*	
*	coordinates: coordinates of the stripe	
*   snakes: array representing the unit matrix
*
*	returns: void
*/
void set_ustripe(Point coordinates, uint16_t val, uint16_t *snakes){
    snakes[get_ustripe_index(coordinates)] = val;
}

/*
*	Function:	set_unit
*	--------------------
*	set the value of a unit in a unit array
*   (a unit being a 2-bit value representing a direction
*   a specific segment of a snake is moving when inhabiting
*   a specific point)
*	
*	coordinates: coordinates of the unit	
*   snakes: array representing the unit matrix
*
*	returns: void
*/
void set_unit(Point coordinates, uint8_t val, uint16_t *snakes){
    uint8_t index = get_unit_index(coordinates.y) * UNIT_SIZE;
    insert_ubit(index, val, snakes + unit_to_ustripe(coordinates));
    insert_ubit(index + 1, val >> 1, snakes + unit_to_ustripe(coordinates));
}

/*
*	Function:	move_segment
*	------------------------
*	Move a snake segment a specific direction
*   (a snake segment being a column of the snake)
*   with the length of the global variable SEGMENT_SIZE)
*   
*	dir: direction to move the segment
*	segment: pointer to segment to move	
*   distance: distance in pixels to move the segment
*
*	returns: void
*/
void move_segment(direction dir, Point* segment, uint8_t distance){
    int i;
    switch (dir)
    {
    case Right:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (segment[i].x + distance) % STRIPE_COLS;
        break;
    case Left:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].x = (uint8_t)(segment[i].x - distance) % STRIPE_COLS;
        break;
    case Up:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (uint8_t)(segment[i].y - distance) % ROWS;
        break;
    case Down:
        for(i = 0; i < SEGMENT_SIZE; ++i) segment[i].y = (segment[i].y + distance) % ROWS;
        break;
    }
}

/*
*	Function:	set_segment
*	------------------------
*	make one snake segment equal to another by copying
*   (a snake segment being a column of the snake)
*   with the length of the global variable SEGMENT_SIZE)
*
*   left: segment to store the copied segment in
*   right: segment to copy
*
*	returns: void
*/
void set_segment(Point *left, const Point *right){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) left[i] = right[i];
}

/*
*	Function:	get_moved
*	---------------------
*	get a copy of a snake segment after it's been moved
*   without actually moving the original segment 
*   (a snake segment being a column of the snake)
*   with the length of the global variable SEGMENT_SIZE)
*
*   dir: direction to move segment
*   segment: original sigmant to move
*   distance: distance to move the segment in pixels
*   res: variable to store the resulting copy of 
*   the original segment after it's been moved
*
*	returns: void
*/
void get_moved(direction dir, const Point* segment, uint8_t distance, Point* res){
    set_segment(res, segment);
    move_segment(dir, res, distance);
}

/*
*	Function:	toggle_segment
*	--------------------------
*	toggle the pixel on the screen that a snake segment
*   inhabits on or off
*   (a snake segment being a column of the snake)
*   with the length of the global variable SEGMENT_SIZE)
*
*   frame: array representing a screen matrix
*   segment: segment specifying witch pixels to toggle on or off
*   (depending on it's coordinates and SEGMENT_SIZE) 
*   stat: wether to turn the pixel on or off
*
*	returns: void
*/
void toggle_segment(uint8_t *frame, Point *segment, pixel_status stat){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_pixel(frame, segment[i], stat);
}

/*
*	Function:	update_segment
*	--------------------------
*	update the units in a unit array that a snake segment
*   inhabits to a specific direction
*   (a snake segment being a column of the snake)
*   with the length of the global variable SEGMENT_SIZE)
*
*   segment: segment specifying witch units to update
*   (depending on it's coordinates and SEGMENT_SIZE) 
*   dir: witch direction to update the segment with
*   snakes: array representing the unit matrix
*
*	returns: void
*/
void update_segment(Point *segment, direction dir, uint16_t *snakes){
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i) set_unit(segment[i], dir, snakes);
}

/*
*	Function:	point_equal
*	--------------------------
*   check if two points are equal (have the same
*   x and y coordinates)	
*
*   left: first point to compare
*   right: second point to compare
*
*	returns: boolean value of wether the points
*   were equal or not
*/
uint8_t point_equal(const Point *left, const Point *right){
    return left->x % STRIPE_COLS == right->x % STRIPE_COLS && left->y % ROWS == right->y % ROWS;
}

/*
*	Function:	is_food
*	-------------------
*   check if a point inhabits a food object
*   on the screen where a food object is a square
*   with same width as SEGMENT_SIZE
*
*   pos: point to check if it inhabits the food object
*   food_pos: coordinates off the upper left point 
*   of where the food object currently is
*
*	returns: boolean value of wether the point inhabits
*   the food object or not
*/
uint8_t is_food(Point pos, Point food_pos){
    int i, j;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        for(j = 0; j < SEGMENT_SIZE; ++j){
            if(point_equal(&pos, &(Point){food_pos.x + j, food_pos.y + i}))
                return 1;
        }
    }
    return 0;
}

/*
*	Function:	toggle_food
*	-----------------------
*   toggle food on or of on ths screen
*
*   frame: array representing a screen matrix
*   stat: wether to turn the pixel on or off
*   food_pos: coordinates off the upper left point 
*   of where the food object currently is
*
*	returns: void
*/
void toggle_food(uint8_t *frame, pixel_status stat, Point const *food_pos){
    int i, j;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        for(j = 0; j < SEGMENT_SIZE; ++j){
            Point fpos = {(food_pos->x + j) %  STRIPE_COLS, (food_pos->y + i) % ROWS};
            set_pixel(frame, fpos, stat);
        }
    }   
}

/*
*	Function:	update_food
*	-----------------------
*   update the foods position after being eaten
*   so that it can respawn in a new empty spot
*
*   frame: array representing a screen matrix
*   coordinates: wether to turn the pixel on or off
*   food_pos: variable to store coordinates off the upper left point 
*   of where the food ets moved
*
*	returns: wether the screen is to occupied to find an empty spot for
*   food (player has reached max length taking up the entire screen)
*/
uint8_t update_food(uint8_t *frame, Point coordinate, Point *food_pos){
    Point original = coordinate;
    uint8_t cur_x = coordinate.x - coordinate.y;
    uint8_t max_x = STRIPE_COLS - SEGMENT_SIZE + 1;
    uint8_t max_y = ROWS - SEGMENT_SIZE + 1;

    int i, j;
    uint8_t invalid_pos = 0;
    while(1){
        for(i = 0; i < SEGMENT_SIZE; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                Point fpos = {(coordinate.x + j) % STRIPE_COLS, (coordinate.y + i) % ROWS};
                if(get_pixel(frame, fpos)){
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
            *food_pos = coordinate;
            return 0;
        }    
    }
}

/*
*	Function:	eat
*	---------------
*   change the screen matrix accordingly whenever
*   a snake eats food
*
*   head: segment of the snake representing it's head
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*
*	returns: void
*/
void eat(Point *head, Point *food_pos, uint16_t *snakes, uint8_t *frame){
    int i, j;
    direction dir = get_unit(*head, snakes);
    toggle_food(frame, Off, food_pos);
    for(i = 1; i < SEGMENT_SIZE; ++i){
        update_segment(head, dir, snakes);
        toggle_segment(frame, head, On);
        move_segment(get_unit(*head, snakes), head, 1);
    }
    update_segment(head, dir, snakes);
    toggle_segment(frame, head, On);
}

/*
*	Function:	current_sstate
*	--------------------------
*   Get the current state of the snake based on the current
*   state of the game
*
*   head: segment of the snake representing it's head
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   frame: array representing a screen matrix
*
*	returns: the current state of the snake
*/
snake_state current_sstate(const Point *head, Point food_pos, uint8_t *frame){ 
    int i;
    snake_state ret = Alive;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        if(is_food(head[i], food_pos)) ret = Ate;
        else if(get_pixel(frame, head[i])) return Dead;
    }
    return ret;
}

/*
*	Function:	status_ahead
*	------------------------
*   Get the state state of the snake if it where to
*   move a certain direction a certain number of pixels
*   from where it currently is. usefull for the snake AI
*
*   dir: direction to check the future state of
*   segment: position of the snakes current head
*   segment and the pixels it inhabits
*   distance: distance to check future states of
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   frame: array representing a screen matrix
*
*	returns: the resulting future state of the snake relative to 
*   the current game state
*/
snake_state status_ahead(direction dir, const Point* segment, uint8_t distance, Point food_pos, uint8_t *frame){
    int i;
    snake_state state;
    Point copy[SEGMENT_SIZE];
    get_moved(dir, segment, distance, copy);
    return current_sstate(copy, food_pos, frame);
}

/*
*	Function:	rotate_segment
*	--------------------------
*   rotate a segment of the snake a specific direction
*   for when it needs to turn the tail or head segments
*
*   current_dir: direction the segment is currently moving
*   rotation: direction to rotate/turn the segment
*   segment: the segment to rotate (usually a head or tail segment)
*
*	returns: boolean value of wether the snake was able to rotate that direction
*   or not (for example if you rotate the same or opposite direction
*   of the direction the snake was already going) 
*   (1 = not able to rotate, 0 = able to rotate)
*/
uint8_t rotate_segment(direction current_dir, direction rotation, Point *segment){
    int i;
    /*Could have wrote shorter and less repetitive code by putting the switch statement
    inside the for loop. However this is technically slower as it 
    would have to check the switch every iteration of the loop rather
    than just once in the beginning*/
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

/*
*	Function:	get_rotated
*	-----------------------
*   get a copy off a segment after it's been rotated
*   without actually rotating the original segment
*
*   current_dir: direction the segment is currently moving
*   rotation: direction to rotate/turn the segment
*   segment: the segment to rotate (usually a head or tail segment)
*   ret: variable to store the copy of the rotated original segment
*   into
*
*	returns: boolean value of wether the snake was able to rotate that direction
*   or not (for example if you rotate the same or opposite direction
*   of the direction the snake was already going) 
*   (1 = not able to rotate, 0 = able to rotate)
*/
uint8_t get_rotated(direction current_dir, direction rotation, const Point *segment, Point* ret){
    int i;
    set_segment(ret, segment);
    if(rotate_segment(current_dir, rotation, ret)) return 1;
    for(i = 1; i < SEGMENT_SIZE; ++i){
        move_segment(rotation, ret, 1);
    } 
    return 0;
}

/*
*	Function:	change_dir
*	----------------------
*   do the operations necessary to fully change the direction
*   a segment is moving in through unit array. if you 
*   try to change to an invalid direction 
*   (same as original  or opposite, nothing happens)
*
*   dir: new direction for segment to move
*   head: where the head currently is on the screen
*   snakes: array representing the unit matrix 
*
*	returns: void
*/
void change_dir(direction dir, Point *head, uint16_t *snakes){
    int i;
    if(rotate_segment(get_unit(*head, snakes), dir, head)) return;  
    for(i = 1; i < SEGMENT_SIZE; ++i){
        update_segment(head, dir, snakes);
        move_segment(get_unit(*head, snakes), head, 1);
    } 
    update_segment(head, dir, snakes);
}   

/*
*	Function:	irand
*	------------------
*   generate a random int based on a snakes
*   current tail and head position aswell as separate
*   timers
*
*   tail: current tail of a snake
*   head: current head of a snake
*
*	returns: randomly generated int
*/
int irand(Point *tail, Point *head){
    return TMR3 - TMR4 + tail->x*head->y + head->x + tail->y ;
}


/*
*	Function:	irand
*	------------------
*   generate a random point based on a snakes
*   current tail and head position aswell as separate
*   timers
*
*   tail: current tail of a snake
*   head: current head of a snake
*
*	returns: randomly generated point
*/
Point prand(Point *tail, Point *head){
    Point ret;
    ret.x = ((TMR3 + tail->x * head->x) % (STRIPE_COLS / SEGMENT_SIZE)) * SEGMENT_SIZE;
    ret.y = ((TMR4 + tail->y * head->y) % (ROWS / SEGMENT_SIZE)) * SEGMENT_SIZE;
    return ret;
}

/*
*	Function:	update_tail_dir
*	---------------------------
*   update tail in the unit array when the snake moves
*   based on the next segments direction in order
*   to have tail properly receed in the same direction 
*   as the snake segments in front
*
*   tail: current tail of a snake
*   snakes: array representing the unit matrix 
*
*	returns: void
*/
void update_tail_dir(Point *tail, uint16_t *snakes){
    Point prev_tail[SEGMENT_SIZE];
    set_segment(prev_tail, tail); 
    
    move_segment(get_unit(*tail, snakes), tail, 1);
    int i;
    for(i = 0; i < SEGMENT_SIZE; ++i){
        uint8_t prev_tail_dir = get_unit(prev_tail[i], snakes);
        uint8_t cur_tail_dir = get_unit(tail[i], snakes);
        if(prev_tail_dir != cur_tail_dir){
            move_segment(prev_tail_dir, tail, SEGMENT_SIZE - 1);
            rotate_segment(prev_tail_dir, cur_tail_dir, tail);
            break;
        }
    }
}

/*
*	Function:	update_snake
*	-------------------------
*   update a snake on the next frame
*   without updating the tail segment in the unit array
*
*   head: current head of a snake
*   tail: current tail of a snake
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   grow: wether the snake is currently growin in the 
*   game state
*   snakes: array representing the unit matrix 
*   frame: array representing a screen matrix
*
*	returns: void
*/
snake_state update_snake(Point *head, Point *tail, Point *food_pos, uint8_t *grow, uint16_t *snakes, uint8_t *frame){
    int i;
    direction head_dir = get_unit(*head, snakes);
    move_segment(get_unit(*head, snakes), head, 1);
    snake_state ret = current_sstate(head, *food_pos, frame);
    if(ret == Dead) return ret; 
    if(ret == Ate){ 
        ret = Ate;
        *grow += SEGMENT_SIZE;
        toggle_food(frame, Off, food_pos);
    }
    if(*grow){
        if(ret != Ate) ret = Growing;
        *grow = *grow - 1;
        toggle_segment(frame, head, On);
        update_segment(head, head_dir, snakes); 
        return ret;
    }

    toggle_segment(frame, tail, Off);
    toggle_segment(frame, head, On);
    update_segment(head, head_dir, snakes);

    //uint8_t prev_tail_dir = get_unit(*tail);
    /*if(cur_tail_dir != prev_tail_dir){
        move_segment(prev_tail_dir, tail, SEGMENT_SIZE - 1);
        rotate_segment(prev_tail_dir, cur_tail_dir, tail);
    }*/
    return ret;
}

/*
*	Function:	update_player_snake
*	-------------------------------
*   fully update player snake on the next frame
*   for both the unit array, the frame array, head
*   and tail
*
*   head: current head of a snake
*   tail: current tail of a snake
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   grow: wether the snake is currently growin in the 
*   game state
*   snakes: array representing the unit matrix 
*   frame: array representing a screen matrix
*
*	returns: void
*/
snake_state update_player_snake(Point *head, Point *tail, Point *food_pos, uint8_t *grow, uint16_t *snakes, uint8_t *frame){
    snake_state state = update_snake(head, tail, food_pos, grow, snakes, frame);
    if(state == Alive) update_tail_dir(tail, snakes);
    return state;
}

/*
*	Function:	update_ai_snake
*	---------------------------
*   fully update player snake on the next frame
*   for both the unit array, the frame array, head
*   and tail
*
*   head: current head of a snake
*   tail: current tail of a snake
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   grow: wether the snake is currently growin in the 
*   game state
*   snakes: array representing the unit matrix 
*   frame: array representing a screen matrix
*   tracker: frame array that only contains the updated
*   AI snake so that it can be cleared from the screen upon it's death
*   without disturbing any other element on the screen   
*
*	returns: void
*/
snake_state update_ai_snake(Point *head, Point *tail, Point *food_pos, uint8_t *grow, uint16_t *snakes, uint8_t *frame, uint8_t *tracker){
    snake_state state = update_snake(head, tail, food_pos, grow, snakes, frame);
    switch (state)
    {
    case Growing: case Ate:
        toggle_segment(tracker, head, On);
        break;
    case Alive:
        toggle_segment(tracker, tail, Off);
        toggle_segment(tracker, head, On);
        update_tail_dir(tail, snakes);
    default:
        break;
    }
    return state;
}

/*
*	Function:	spawn_snake
*	---------------------------
*   spawn a snake on the screen at a starting position 
*   with optional length
*
*   tail: current tail of a snake
*   head: current head of a snake
*   len: length of newly spawned snake
*   dir: direction for it to go to begin with
*   snakes: array representing the unit matrix 
*   frame: array representing a screen matrix
*
*	returns: void
*/
void spawn_snake(Point *tail, Point *head, uint8_t len, direction dir, uint16_t *snakes, uint8_t *frame){
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
        toggle_segment(frame, head, On);
        update_segment(head, dir, snakes);
        move_segment(dir, head, 1);
    }
    toggle_segment(frame, head, On);
    update_segment(head, dir, snakes);
}

/*
*	Struct:	Distance
*	---------------------------
*   distance of a snake segment from a food 
*   and or obsticals when going a specific
*   direction from where it is currently
*
*   dir: direction the distance is relevant to
*   food_dist: distance to food
*   obstical_dist: distance to nearest obstical
*/
typedef struct Distance{
    direction dir;
    int food_dist;
    int obstical_dist;
} Distance;

/*
*	Function:	swap_dist
*	---------------------------
*   swap two distances. Relevant for sorting purposes 
*
*   right: first distance to swap
*   left: second distance to swap
*
*	returns: void
*/
void swap_dist(Distance *right, Distance *left){
    Distance temp = *right;
    *right = *left;
    *left = temp;
}

/*
*	Function:	dist_fsort
*	---------------------------
*   sort a distance array according to 
*   the food distance from smallest to greatest
*
*   dists: distance array to sort
*   len: length of distance array to sort
*
*	returns: void
*/
void dist_fsort(Distance *dists, int len){
    int i, j;
    for(i = 1; i < len; ++i){
        for (j = i; 0 < j && dists[j].food_dist < dists[j - 1].food_dist; --j)
            swap_dist(dists + j, dists + j - 1);
    }
}

/*
*	Function:	dist_fsort
*	---------------------------
*   sort a distance array according to 
*   the obstical distance from greatest to smallest
*
*   dists: distance array to sort
*   len: length of distance array to sort
*
*	returns: void
*/
void dist_osort(Distance *dists, int len){
    int i, j;
    for(i = 1; i < len; ++i){
        for (j = i; 0 < j &&  dists[j - 1].obstical_dist < dists[j].obstical_dist; --j)
            swap_dist(dists + j, dists + j - 1);
    }
}

/*
*	Function:	obstacle_distance
*	-----------------------------
*   check distance to nearest obstacle from current
*   snake head position going a specific direction
*
*   dir: direction to check for obstables
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   check_dists: distance in x and y directions to check
*   for obstacles
*   frame: array representing a screen matrix
*
*	returns: distance to closest obstacle in specific direction
*/
int obstacle_distance(direction dir, const Point* head, Point food_pos, Point check_dists, uint8_t *frame){
    int i;
    int distance;
    switch (dir){
    case Up: case Down:
        distance = check_dists.y;
        break;
    case Left: case Right:
        distance = check_dists.x;
        break;
    }
    for(i = 0; i < distance; ++i){
        snake_state state = status_ahead(dir, head, i + 1, food_pos, frame);
        switch (state){
        case Dead:
            return i + 1;
        //if snake will reach the food in this direction before any obstacle appears
        case Ate: return 255;
        default:
            break;
        }
    }
    // if there is no obstacle
    return 254;
}

/*
*	Function:	obstacle_distance
*	-----------------------------
*   function for snake AI algorithm that essentially 
*   runs the entire snake AI. difficulty can be adjusted by
*   for example decreasing how far ahead it checks for obstacles
*   
*   head: current head of a snake
*   food_pos: coordinates off the upper left point of
*   where the food currently is
*   check_dists: distance in x and y directions to check
*   for obstacles
*   snakes: array representing the unit matrix 
*   frame: array representing a screen matrix
*
*	returns: direction to move the snake ai in the next movement frame
*/
direction snake_ai(Point *head, Point food_pos, Point check_dists, uint16_t* snakes, uint8_t *frame){
    direction ret;
    Distance dir_dists[3];
    
    uint8_t arrived_x = 0;
    uint8_t arrived_y = 0;
    int i; 

    for(i = 0; i < SEGMENT_SIZE; ++i){
        if(head[i].y == food_pos.y || head[i].y == food_pos.y + 1){
            arrived_y = 1;
        }
        if(head[i].x == food_pos.x || head[i].x == food_pos.x + 1){
            arrived_x = 1;
        }
    }
    direction ori_dir = get_unit(*head, snakes); 

    /*Could have wrote shorter and less repetitive code by putting the switch statement
    inside the for loop. However this is technically slower as it 
    would have to check the switch every iteration of the loop rather
    than just once in the beginning*/

    //check the distances to food in all viable directions and put them in a list
    switch (ori_dir)
    {
    case Left:{
        if(arrived_y){
            dir_dists[0] = (Distance){Down, 255};
            dir_dists[1] = (Distance){Up, 255};
        }
        else if(food_pos.y < head->y){
            dir_dists[0] = (Distance){Down, food_pos.y + (32 - head[SEGMENT_SIZE - 1].y)};
            dir_dists[1] = (Distance){Up, head->y - food_pos.y - (SEGMENT_SIZE - 1)};
        }
        else{
            dir_dists[0] = (Distance){Up, head->y + (32 - food_pos.y - (SEGMENT_SIZE - 1))};
            dir_dists[1] = (Distance){Down, food_pos.y - head[SEGMENT_SIZE - 1].y};
        }
        if(arrived_x) dir_dists[2] = (Distance){Left, 255};
        else if(food_pos.x < head->x)
            dir_dists[2] = (Distance){Left, head->x - food_pos.x - (SEGMENT_SIZE - 1)};
        else
            dir_dists[2] = (Distance){Left, head->x + (128 - food_pos.x) - (SEGMENT_SIZE - 1)};
        break;
        
    }
    case Right:{
        if(arrived_y){
            dir_dists[0] = (Distance){Down, 255};
            dir_dists[1] = (Distance){Up, 255};
        }
        else if(food_pos.y < head->y){
            dir_dists[0] = (Distance){Down, food_pos.y + (32 - head[SEGMENT_SIZE - 1].y)};
            dir_dists[1] = (Distance){Up, head->y - food_pos.y - (SEGMENT_SIZE - 1)};
        }
        else{
            dir_dists[0] = (Distance){Up, head->y + (32 - food_pos.y - (SEGMENT_SIZE - 1))};
            dir_dists[1] = (Distance){Down, food_pos.y - head[SEGMENT_SIZE - 1].y};
        }
        if(arrived_x) dir_dists[2] = (Distance){Right, 255};
        else if(head->x < food_pos.x)
            dir_dists[2] = (Distance){Right, food_pos.x - head->x};
        else
            dir_dists[2] = (Distance){Right, food_pos.x + (128 - head->x)};
        break; 
    }
    case Up:{
        if(arrived_x){
            dir_dists[0] = (Distance){Right, 255};
            dir_dists[1] = (Distance){Left, 255};
        }
        else if(food_pos.x < head->x){
            dir_dists[0] = (Distance){Right, food_pos.x + (128 - head[SEGMENT_SIZE - 1].x)};
            dir_dists[1] = (Distance){Left, head->x - food_pos.x - (SEGMENT_SIZE - 1)};
        }
        else{
            dir_dists[0] = (Distance){Left, head->x + (128 - food_pos.x) - (SEGMENT_SIZE - 1)};
            dir_dists[1] = (Distance){Right, food_pos.x - head[SEGMENT_SIZE - 1].x};
        }
        if(arrived_y) dir_dists[2] = (Distance){Up, 255};
        else if(food_pos.y < head->y)
            dir_dists[2] = (Distance){Up, head->y - food_pos.y - (SEGMENT_SIZE - 1)};
        else
            dir_dists[2] = (Distance){Up, food_pos.y + (32 - head->y) - (SEGMENT_SIZE - 1)};
        break; 
    }
    case Down:{
        if(arrived_x){
            dir_dists[0] = (Distance){Right, 255};
            dir_dists[1] = (Distance){Left, 255};
        }
        else if(food_pos.x < head->x){
            dir_dists[0] = (Distance){Right, food_pos.x + (128 - head[SEGMENT_SIZE - 1].x)};
            dir_dists[1] = (Distance){Left, head->x - food_pos.x - (SEGMENT_SIZE - 1)};
        }
        else{
            dir_dists[0] = (Distance){Left, head->x + (128 - food_pos.x) - (SEGMENT_SIZE - 1)};
            dir_dists[1] = (Distance){Right, food_pos.x - head[SEGMENT_SIZE - 1].x};
        }
        if(arrived_y) dir_dists[2] = (Distance){Down, 255};
        else if(head->y < food_pos.y)
            dir_dists[2] = (Distance){Down, food_pos.y - head->y};
        else
            dir_dists[2] = (Distance){Down, food_pos.y + (32 - head->y)};
        break; 
    }
    default:
        break;
    }
    //sort the list of distances to food from smallest distance to largest
    dist_fsort(dir_dists, 3);
    for(i = 0; i < 3; ++i){
        Point ret[SEGMENT_SIZE];
        get_rotated(ori_dir, dir_dists[i].dir, head, ret);
        //get the obstacle distance aswell in this direction
        dir_dists[i].obstical_dist = obstacle_distance(dir_dists[i].dir, ret, food_pos, check_dists, frame);

        if(dir_dists[i].obstical_dist > 128) return dir_dists[i].dir;
    }
    /*if all directions have obstacles before food sort the list by obstacle distances
    *from greatest to smallest*/
    dist_osort(dir_dists, 3);
    return dir_dists[0].dir;
}















/*
    for(i = 0; i < check_dist; ++i){
        if(segment->x != food_pos.x && segment->x != food_pos.x + 1){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                switch (dir)
                {
                case Left:{
                    if(get_pixel((Point){(uint8_t)(segment[i].x - i) % 128, segment[i].y}))
                        return i;
                    break;
                }
                case Right:{
                    if(get_pixel((Point){(segment[i].x + i) % 128, segment[i].y}))
                        return i;
                    break;
                }
                case Up:{
                    if(get_pixel((Point){segment[i].x, (uint8_t)(segment[i].y - i) % 32}))
                        return i;
                    break;
                }
                case Down:{
                    if(get_pixel((Point){segment[i].x, (segment[i].y + i) % 32}))
                        return i;
                    break;
                }
                default:
                    break;
                }
            }
        }
        else return 129;
    }
    */









/*
int obstacle_distance2(direction dir, const Point* head, Point food_pos, Point check_dists, uint8_t *frame){
    int i;
    int j;
    uint8_t food = 0;
    uint8_t passed = 0;
    uint8_t loop = 0;
    switch (dir)
    {
    case Right:{
        for(i = 1; i <= check_dists.x; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                Point current = {(head[j].x + i) % 128, head[j].y};
                if(is_food(current, food_pos))
                    food = 1;
                //else if(is_segment(current, tail) && get_unit(*tail, snakes) == Right)
                  //  loop = 1;
                else{ 
                    if(get_pixel(frame, current))
                        return i;
                   // else if(distance + (SEGMENT_SIZE - 1) < i) passed = 1;
                }
            }
            if(food || passed || loop) return 255;
        }
        break;
    }
    case Left:{
        for(i = 1; i <= check_dists.x; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                Point current = {(uint8_t)(head[j].x - i) % 128, head[j].y};
                if(is_food(current, food_pos))
                    food = 1;
                //else if(is_segment(current, tail) && get_unit(*tail, snakes) == Left)
                  //  loop = 1;
                else{ 
                    if(get_pixel(frame, current))
                        return i;
                   // else if(distance + (SEGMENT_SIZE - 1) < i) passed = 1;
                }
            }
            if(food || passed || loop) return 255;
        }
        break;
    }
    case Up:{
        for(i = 1; i <= check_dists.y; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                
                Point current = {head[j].x, (uint8_t)(head[j].y - i) % 32};
                
                if(is_food(current, food_pos))
                    food = 1;
                //else if(is_segment(current, tail) && get_unit(*tail, snakes) == Up)
                  //  loop = 1;
                else{ 
                    if(get_pixel(frame, current)){
                        return i;
                    }
                   // else if(distance + (SEGMENT_SIZE - 1) < i) passed = 1;
                }
            }
            if(food || passed || loop){
                return 255;
            }
        }
        break;
    }
    case Down:{
        for(i = 1; i <= check_dists.y; ++i){
            for(j = 0; j < SEGMENT_SIZE; ++j){
                Point current = {head[j].x, (head[j].y + i) % 32};
                if(is_food(current, food_pos))
                    food = 1;
                //else if(is_segment(current, tail) && get_unit(*tail, snakes) == Down)
                  //  loop = 1;
                else{ 
                    if(get_pixel(frame, current))
                        return i;
                   // else if(distance + (SEGMENT_SIZE - 1) < i) passed = 1;
                }
            }
            if(food || passed || loop) return 255;
        }
        break;
    }
    default:
        break;
    }
    return 254;
}
*/