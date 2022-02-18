#include "u32graphics.h"
#include "data.h"

#define STRIPE_COLS 128
#define STRIPE_BIT_LEN 8
#define STRIPE_ROWS 4
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

uint8_t SCREEN[128*4];


//matrix for SCREEN in column major; stored as a unsigned char array

void clear_frame(uint8_t *frame){
	int i;
	for(i = 0; i < SCREEN_LEN; ++i){
		frame[i] = 0;
	}
}

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

int pixel_to_stripe(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 32) / STRIPE_BIT_LEN;
} 
int get_stripe_index(Point coordinates){
    return (coordinates.x % STRIPE_COLS) * STRIPE_ROWS + (coordinates.y % 4);
}
int get_pixel_index(uint8_t y){
    return (y % 32) % STRIPE_BIT_LEN;
}
uint8_t get_stripe(uint8_t *frame, Point coordinates){
    return frame[get_stripe_index(coordinates)];
}

uint8_t get_pixel(uint8_t *frame, Point coordinates){
    uint8_t stripe = frame[pixel_to_stripe(coordinates)];
    return (stripe >> get_pixel_index(coordinates.y)) & 0x01;
}

void set_stripe(uint8_t *frame, Point coordinates, uint8_t val){
    frame[get_stripe_index(coordinates)] = val;
}

void set_pixel(uint8_t *frame, Point coordinates, uint8_t val){
    uint8_t index = get_pixel_index(coordinates.y);
    insert_bit(index, val, frame + pixel_to_stripe(coordinates));
}
Point chcoord_to_stcoord(Point coordinates){
	return (Point){coordinates.x * 8, coordinates.y};
}

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



void write_char(uint8_t *frame, Point chcoord, char ch){
	int i;
	int c = ch;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe(frame, (Point){chcoord.x + i, chcoord.y}, FONT[c * 8 + i]);
	}
}
void invert_char(uint8_t *frame, Point chcoord){
	int i;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe(frame, (Point){chcoord.x + i, chcoord.y}, ~get_stripe(frame, (Point){chcoord.x + i, chcoord.y}));
	}
}

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
void invert_row(uint8_t *frame, uint8_t line){
	line %= 4;
	int i, j;
	for(i = 0; i < 16; ++i){
		invert_char(frame, (Point){i, line});
	}
}

void write_string(uint8_t *frame, uint8_t line, char const *str, uint8_t len){
	line %= 4;
	int i;
	for(i = 0; i < len; ++i){
		if(*str & 0x80) continue;
		write_char(frame, (Point){i, line}, *str);
		str++;
	}
}
void invert_string(uint8_t *frame, uint8_t line, uint8_t len){
	line %= 4;
	int i, j;
	for(i = 0; i < len; ++i){
		invert_char(frame, (Point){i, line});
	}
}

Point normalize(uint8_t width, uint8_t height, Point coords){
	return (Point){coords.x % width, coords.y % height};
}

void rotate_point90(Point *point, uint8_t turns, uint8_t clockwise){
	Point original = {point->x, -point->y + 31};
	int i;
	if(clockwise){
		for(i = 0; i < turns; ++i){
			point->x = original.y;
			point->y = -original.x;  
			original = (Point){point->x, point->y};
		}
	}
	else{
		for(i = 0; i < turns; ++i){
			point->x = -original.y;
			point->y = original.x;  
			original = (Point){point->x, point->y};
		}
	}
	*point = (Point){original.x % 128, original.y % 32};
}

 //pos = where the top left pixel is

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

uint8_t get_image_pixel(Point coords, const Image *image){
	Point normalized = normalize(image->width, image->height, coords);
	int index = get_image_sindex(normalized, image);
	return (image->image[index] >> normalized.y % STRIPE_BIT_LEN) & 0x1;
}

void set_image_pixel(Point coords, uint8_t val, const Image *image){
	Point normalized = normalize(image->width, image->height, coords);
	int index = get_image_sindex(normalized, image);
	insert_bit(normalized.y % STRIPE_BIT_LEN, val, image->image + index);
}



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

void swap_im_pixel(Image *image, Point lcoords, Point rcoords){
	uint8_t temp = get_image_pixel(lcoords, image);
	set_image_pixel(rcoords, get_image_pixel(lcoords, image), image);
	set_image_pixel(rcoords, temp, image);
}

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
void copy_image(const Image* const original, Image* copy){
	int i;
	copy->height = original->height;
	copy->width = original->width;
	copy->im_major = original->im_major;
	for(i = 0; i < original->height * original->width; ++i){
		copy->image[i] = original->image[i];	
	}
}

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

void draw_image_rotated(uint8_t *frame, const Image *image, Point pos, uint8_t turns, uint8_t clockwise){
	int i, j;
	Image rotated_image;
	uint8_t im[image->height*image->width];
	rotated_image.image = im;
	copy_image(image, &rotated_image);
	rotate_image(&rotated_image, turns, clockwise);
	draw_image(frame, &rotated_image, pos);
}

void draw_foreground(uint8_t *frame, const Image *image, Point pos, uint8_t forground_mode){
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

void draw_foreground_rotated(uint8_t *frame, const Image *image, Point pos, uint8_t forground_mode, uint8_t turns, uint8_t clockwise){
	int i, j;
	Image rotated_image;
	uint8_t im[image->height*image->width];
	rotated_image.image = im;
	copy_image(image, &rotated_image);
	rotate_image(&rotated_image, turns, clockwise);
	draw_foreground(frame, &rotated_image, pos, forground_mode);
}

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