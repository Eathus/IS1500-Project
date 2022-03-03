#include "u32graphics.h"
#include "data.h"

//How many stripes columns per row on the screen
#define STRIPE_COLS 128
//bit/pixel length of a stripe
#define STRIPE_BIT_LEN 8
//how many rows of tripes the physcal screen has
#define STRIPE_ROWS 4
//total stripes on physical screen
#define SCREEN_LEN 128 * 4

//defines copied from lab 3
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

//Array representing screen matrix everything will appear on
uint8_t SCREEN[128*4];

/*
*	Function:	clear_frame
*	-----------------------
*	clears a frame (turns all teh pixels off)
*
*	frame:  array representing a screen matrix
*
*	returns: void
*/
void clear_frame(uint8_t *frame){
	int i;
	for(i = 0; i < SCREEN_LEN; ++i){
		frame[i] = 0;
	}
}

/*
*	Function:	insert_bit
*	----------------------
*	make any bit light up or turn of in a byte
*
*	pos: witch bit starting from right most bit
* 	new_bit: turn the bit on (1) or off (0)
*	ch: pointer to variable to insert bit into
*
*	returns: void
*/
void insert_bit(uint8_t pos, uint8_t new_bit, uint8_t * ch){
    new_bit &= 0x01;
    uint8_t mask;
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
*	Function:	pixel_to_stripe
*	---------------------------
*	get the index of the 8 bit char representing an 8 pixel
* 	long column on the screen that a specific 
* 	pixel inhabits
*
*	coordinates:	position of the pixel
*	
*	returns: the index of the char in the SCREEN array 
*	witch the pixel inhabits 
*/
int pixel_to_stripe(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 32) / STRIPE_BIT_LEN;
}

/*
*	Function:	get_stripe_index
*	----------------------------
*	get the index of the 8 bit char representing an 8 pixel
* 	long column on the screen using stripe coordinates
*
*	coordinates: coordinate of the stripe 
*	(0 <= x <= 127, 0 <= y <= 4)
*	
*	returns: the index of the char in the SCREEN array 
*	with the specific stripe coordinates
*/
int get_stripe_index(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 4);
}

/*
*	Function:	get_pixel_index
*	---------------------------
*	get the index of the bit in a specific stripe 
*	(8-bit char) representing a pixel on the screen
*
*	y: y coordinate of the pixel since stripes are columns 
*	
*	returns: index of the pixel relative to the stripe
*	it inhabits
*/
int get_pixel_index(uint8_t y){
    return (y % 32) % STRIPE_BIT_LEN;
}

/*
*	Function:	get_stripe
*	----------------------
*	get the value of a stripe in a frame array 
*	
*	frame:  array representing a screen matrix
*	coordinates: coordinates of the stripe	
*
*	returns: value of a stripe (8-bit char)
*	representing an 8 pixel column in a frame
*/
uint8_t get_stripe(uint8_t *frame, Point coordinates){
    return frame[get_stripe_index(coordinates)];
}

/*
*	Function:	get_pixel
*	----------------------
*	get the value of a pixel in a frame array 
*	
*	frame:  array representing a screen matrix
*	coordinates: coordinates of the pixel
*	
*	returns: value of a pixel at specific index
*/
uint8_t get_pixel(uint8_t *frame, Point coordinates){
    uint8_t stripe = frame[pixel_to_stripe(coordinates)];
    return (stripe >> get_pixel_index(coordinates.y)) & 0x01;
}

/*
*	Function:	set_stripe
*	----------------------
*	set the value of a stripe in a frame array 
*	
*	frame:  array representing a screen matrix
*	coordinates: coordinates of the stripe	
*	val: value to set the stripe to
*
*	returns: void
*/
void set_stripe(uint8_t *frame, Point coordinates, uint8_t val){
    frame[get_stripe_index(coordinates)] = val;
}

/*
*	Function:	set_pixel
*	----------------------
*	set the value of a pixel in a frame array 
*	
*	frame:  array representing a screen matrix
*	coordinates: coordinates of the pixel
*	val: value to set the pixel to	
*
*	returns: void
*/
void set_pixel(uint8_t *frame, Point coordinates, uint8_t val){
    uint8_t index = get_pixel_index(coordinates.y);
    insert_bit(index, val, frame + pixel_to_stripe(coordinates));
}

/*
*	Function:	chcoord_to_stcoord
*	------------------------------
*	convert a char coordinate 
*	(one out of 16, 8 pixel squares a letter can inhabit on a 
* 	8 pixel wide row on the screen) to a string
*	coordinate that represent the exact 
* 	starting point (left-top corner) of a char in one 
* 	of the 4 rows making up the screen. Helps
*	with writing out evenely spaced letters on screen  
*	
*	coordinates: char coordinates to convert
*	
*	returns: converted string coordinate
*/
Point chcoord_to_stcoord(Point coordinates){
	return (Point){coordinates.x * 8, coordinates.y};
}

/*
*	Function:	write_char
*	----------------------
*	write a letter on the screen
*	
*	frame:  array representing a screen matrix
*	chcoord: char coordinates 
*	(one out of 16, 8 pixel squares a letter can inhabit on a 
* 	8 pixel wide row on the screen) for the letter
*	
*	returns: void
*/
void write_char(uint8_t *frame, Point chcoord, char ch){
	int i;
	int c = ch;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe(frame, (Point){chcoord.x + i, chcoord.y}, FONT[c * 8 + i]);
	}
}

/*
*	Function:	invert_char
*	-----------------------
*	invert a letter on the screen from black to white foreground
* 	or white to black foreground (opposite background)
*	
*	frame:  array representing a screen matrix
*	chcoord: char coordinates 
*	(one out of 16, 8 pixel squares a letter can inhabit on a 
* 	8 pixel wide row on the screen) for the letter
*	
*	returns: void
*/
void invert_char(uint8_t *frame, Point chcoord){
	int i;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe(frame, (Point){chcoord.x + i, chcoord.y}, ~get_stripe(frame, (Point){chcoord.x + i, chcoord.y}));
	}
}

/*
*	Function:	write_row
*	----------------------
*	write a row of letters on specific row of the screen
*	
*	frame:  array representing a screen matrix
*	line: witch out of 4 rows to write on indexed from top as 0 
*	str: pointer to string to write
*	
*	returns: void
*/
void write_row(uint8_t *frame, uint8_t line, char const *str){
	line %= 4;
	int i;
	for(i = 0; i < 16; ++i){
		if(*str == '\n' || *str == '\0'){
			for(i; i < 16; ++i) write_char(frame, (Point){i, line}, ' ');
			return;
		}
		if(*str & 0x80) continue;
		write_char(frame, (Point){i, line}, *str);
		str++;
	}
}
/*
*	Function:	invert_row
*	----------------------
*	invert a row of letters on specific row of the screen
*	
*	frame:  array representing a screen matrix
*	line: witch out of 4 rows to write on indexed from top as 0 
*	
*	returns: void
*/
void invert_row(uint8_t *frame, uint8_t line){
	line %= 4;
	int i, j;
	for(i = 0; i < 16; ++i){
		invert_char(frame, (Point){i, line});
	}
}

/*
*	Function:	write_string
*	------------------------
*	write a string of letters on specific row of the screen
*	without overwriting the entire row
*	
*	frame:  array representing a screen matrix
*	start: on witch row and position on that row to start writing
*	(0 <= x <= 15), (0 <= y <= 3), x representing character index
*	and y, rwo index
*	str: pointer to string to write
*	len: length of the string to write
*	
*	returns: void
*/
void write_string(uint8_t *frame, Point start, char const *str, uint8_t len){
	start.y %= 4;
	start.x %= 16;
	int i;
	for(i = 0; i < len; ++i){
		if(*str & 0x80) continue;
		write_char(frame, (Point){start.x + i, start.y}, *str);
		str++;
	}
}

/*
*	Function:	invert_string
*	-------------------------
*	invert a string of letters on specific row of the screen
*	without inverting the entire row
*	
*	frame:  array representing a screen matrix
*	line: witch out of 4 rows to write on indexed from top as 0 
*	len: length of the string to write
*	
*	returns: void
*/
void invert_string(uint8_t *frame, uint8_t line, uint8_t len){
	line %= 4;
	int i, j;
	for(i = 0; i < len; ++i){
		invert_char(frame, (Point){i, line});
	}
}

/*
*	Function:	normalize
*	---------------------
*	normalize/fit coordinates according to some max 
*	width and height
*	
*	width:  max width (0 <= x <= width)
*	height:  max height (0 <= y <= height)
*	coords: coordinates to normalize
*	
*	returns: normalized/fitted coordinates
*/
Point normalize(uint8_t width, uint8_t height, Point coords){
	return (Point){coords.x % width, coords.y % height};
}

/*
*	Function:	get_image_sindex
*	----------------------------
*	Get the image array index of a stripe given some 
*	coordinates and an image object
*	
*	coords: coordinates of the pixel 
*	image:  image object to get the index from
*	
*	returns: the index of the stripe in te image array
*	witch contains the pixel coordinates given
*/
int get_image_sindex(Point coords, const Image *image){
	switch (image->im_major)
	{
		case Row:{
			return
				image->width * ((coords.y % image->height) / STRIPE_BIT_LEN) + 
				(coords.x % image->width);
		}
		case Column:{
			return 
				image->height / STRIPE_BIT_LEN * (coords.x % image->width) + 
				(coords.y % image->height) / STRIPE_BIT_LEN;
		}
	}
}

/*
*	Function:	get_image_pixel
*	---------------------------
*	Get a pixel value from a Image object
* 	at specific pixel coordinates
*	
*	coords: coordinates of the pixel 
*	image:  image object to get the pixel value from
*	
*	returns: the pixel value of the image 
*	at the given coordinates
*/
uint8_t get_image_pixel(Point coords, const Image *image){
	Point normalized = normalize(image->width, image->height, coords);
	int index = get_image_sindex(normalized, image);
	return (image->image[index] >> normalized.y % STRIPE_BIT_LEN) & 0x1;
}

/*
*	Function:	set_image_pixel
*	---------------------------
*	aet pixel value for an Image object
* 	at specific pixel coordinates
*	
*	coords: coordinates of the pixel 
*	image:  image object to set the pixel value at
*	
*	returns: void
*/
void set_image_pixel(Point coords, uint8_t val, const Image *image){
	Point normalized = normalize(image->width, image->height, coords);
	int index = get_image_sindex(normalized, image);
	insert_bit(normalized.y % STRIPE_BIT_LEN, val, image->image + index);
}

/*
*	Function:	transpose_image
*	---------------------------
*	Transposes an image for image matrix
*	manipulation purposes
*	
*	image:  image object to transpose
*	
*	returns: the pixel value of the image 
*	at the given coordinates
*/
void transpose_image(Image *image){
	switch (image->im_major) {
        case Row:
            image->im_major = Column;
            break;
        case Column:
            image->im_major = Row;
            break;
    }
    uint8_t temp = image->height;
    image->height = image->width;
    image->width = temp;
}

/*
*	Function:	swap_im_pixel
*	-------------------------
*	swap values of two pixels at an image object
*	for image manipulation purposes
*	
*	image:  image object to transpose
*	lcoords: coordinates of the first pixel to swap
*	rcoords: coordinates of the second pixel to swap
*	
*	returns: void
*/
void swap_im_pixel(Image *image, Point lcoords, Point rcoords){
	uint8_t temp = get_image_pixel(lcoords, image);
	set_image_pixel(rcoords, get_image_pixel(lcoords, image), image);
	set_image_pixel(rcoords, temp, image);
}

/*
*	Function:	reverse_image
*	-------------------------
*	reverse an image object for image manipulation purposes
*	
*	image:  image object to transpose
*	horizontal: reverse the image horizontally (1) or vertically (0)
*	
*	returns: void
*/
void reverse_image(Image *image, uint8_t horizontal){
	int i, j;
	if(horizontal){
		for(i = 0; i < image->height; ++i){
			for(j = 0; j < image->width / 2; ++j){
				swap_im_pixel(image, (Point){j, i}, (Point){image->width - j, i});
			}
		}
	}
	else{
		for(i = 0; i < image->width; ++i){
			for(j = 0; j < image->height / 2; ++j){
				swap_im_pixel(image, (Point){i, j}, (Point){i, image->height - j});
			}
		}
	}
}

/*
*	Function:	rotate_image
*	------------------------
*	rotate an image 90 degrees. For image manipulation purposes
*	
*	image:  image object to transpose
*	turns: how many times to turn
*	clockwise: witch direction to turn image (clockwise = 1, anticlockwise = 0)
*	
*	returns: void
*/
void rotate_image(Image *image, uint8_t turns, uint8_t clockwise){
	int i;
	if(clockwise){
		for(i = 0; i < turns; ++i){
			reverse_image(image, 1);
			transpose_image(image);
		}
	}
	else{
		for(i = 0; i < turns; ++i){
			transpose_image(image);
			reverse_image(image, 1);
		}
	}
}

/*
*	Function:	copy_image
*	----------------------
*	copy an image into a new image object 
*	
*	image:  image object to transpose
*	original: image to copy
*	copy: pointer to image to store copy into
*	
*	returns: void
*/
void copy_image(const Image* const original, Image* copy){
	int i;
	copy->height = original->height;
	copy->width = original->width;
	copy->im_major = original->im_major;
	for(i = 0; i < original->height * original->width; ++i){
		copy->image[i] = original->image[i];	
	}
}

/*
*	Function:	draw_image
*	----------------------
*	draw image on a frame
*	
*	frame:  array representing a screen matrix
*	image:  image object to transpose
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	
*	returns: void
*/
void draw_image(uint8_t *frame, const Image *image, Point pos){
	int i, j;
	for (i = 0; i < image->height; ++i){
		for (j = 0; j < image->width; ++j){
			Point normalized = 
				{((int)pos.x + j) % 128, ((int)pos.y + i) % 32};
			set_pixel(frame, normalized, get_image_pixel((Point){j, i}, image));
		}
	}
}

/*
*	Function:	draw_image_rotated
*	------------------------------
*	draw rotated image on a frame
*	
*	frame:  array representing a screen matrix
*	image:  image object to transpose
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	turns: how many times to turn
*	clockwise: witch direction to turn image (clockwise = 1, anticlockwise = 0)	
*
*	returns: void
*/
void draw_image_rotated(uint8_t *frame, const Image *image, Point pos, uint8_t turns, uint8_t clockwise){
	int i, j;
	Image rotated_image;
	uint8_t im[image->height*image->width];
	rotated_image.image = im;
	copy_image(image, &rotated_image);
	rotate_image(&rotated_image, turns, clockwise);
	draw_image(frame, &rotated_image, pos);
}

/*
*	Function:	draw_foreground
*	---------------------------
*	draw exclusively foreground of image (lit or off pixels)
*	
*	frame:  array representing a screen matrix
*	image:  image object to transpose
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	forground_mode: wether to draw off pixels or on pixels	
*
*	returns: void
*/
void draw_foreground(uint8_t *frame, const Image *image, Point pos, pixel_status forground_mode){
	int i, j;
	forground_mode %= 2;
	for (i = 0; i < image->height; ++i){
		for (j = 0; j < image->width; ++j){
			Point normalized =
				{((int)pos.x + j) % 128, ((int)pos.y + i) % 32};
			uint8_t pixel = get_image_pixel((Point){j, i}, image);
			if(pixel == forground_mode) set_pixel(frame, normalized, pixel);
		}
	}
}

/*
*	Function:	draw_foreground_rotated
*	-----------------------------------
*	draw exclusively foreground of image (lit or off pixels) rotated
*	
*	frame:  array representing a screen matrix
*	image:  image object to transpose
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	forground_mode: wether to draw off pixels or on pixels	
*	turns: how many times to turn
*	clockwise: witch direction to turn image (clockwise = 1, anticlockwise = 0)	
*
*	returns: void
*/
void draw_foreground_rotated(uint8_t *frame, const Image *image, Point pos, pixel_status forground_mode, uint8_t turns, uint8_t clockwise){
	int i, j;
	Image rotated_image;
	uint8_t im[image->height*image->width];
	rotated_image.image = im;
	copy_image(image, &rotated_image);
	rotate_image(&rotated_image, turns, clockwise);
	draw_foreground(frame, &rotated_image, pos, forground_mode);
}

/*
*	Function:	draw_rectangle
*	---------------------------
*	draw a rectangle on a frame
*	
*	frame:  array representing a screen matrix
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	width: width of the rectangle
*	height: height of the rectangle
*	foreground_mode: wether to draw black (off) or white (on) triangle
*
*	returns: void
*/
void draw_rectangle(uint8_t *frame, Point pos, uint8_t width, uint8_t height, pixel_status forground_mode){
	int i, j;
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			set_pixel(frame, (Point){pos.x + j, pos.y + i}, forground_mode);	
}

/*
*	Function:	draw_square
*	-----------------------
*	draw a square on a frame
*	
*	frame:  array representing a screen matrix
*	pos: 	where to draw the image on the frame 
*	(starting from upper left corner of image)
*	width: width of the square sides
*	foreground_mode: wether to draw black (off) or white (on) triangle
*
*	returns: void
*/
void draw_square(uint8_t *frame, Point pos, uint8_t width, pixel_status forground_mode){
	draw_rectangle(frame, pos, width, width, forground_mode);
}

uint8_t spi_send_recv(uint8_t data);

/*
*	Function:	update_disp
*	-----------------------
*	update the physical display with what's in a frame matrix array
*	
*	frame:  array representing a screen matrix
*
*	returns: void
*/
void update_disp(uint8_t *frame){
    uint8_t i, j;
    for(i = 0; i < STRIPE_ROWS; ++i){
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
		spi_send_recv(i);

        spi_send_recv(0x00);
		spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < STRIPE_COLS; ++j)
			spi_send_recv(get_stripe(frame, (Point){j, i}));
    }
}



//copied from Lab 3

void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void init_disp(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

//copied from Lab 3
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num, int *len){
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  *len = ITOA_BUFSIZ - i - 2;
  return( &itoa_buffer[ i + 1 ] );
}

//copied from Lab 3
void num32asc( char * s, int n ){
  int i;
  uint8_t save = 0;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}