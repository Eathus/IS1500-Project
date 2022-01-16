#include "u32graphics.h"
#include "data.h"

#define STRIPE_COLS 128
#define STRIPE_BIT_LEN 8
#define STRIPE_ROWS 4
#define SCREEN_LEN 128 * 4

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

void clear_screen(){
	int i;
	for(i = 0; i < SCREEN_LEN; ++i){
		SCREEN[i] = 0;
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
uint8_t get_stripe(Point coordinates){
    return SCREEN[get_stripe_index(coordinates)];
}

uint8_t get_pixel(Point coordinates){
    uint8_t stripe = SCREEN[pixel_to_stripe(coordinates)];
    return (stripe >> get_pixel_index(coordinates.y)) & 0x01;
}

void set_stripe(Point coordinates, uint8_t val){
    SCREEN[get_stripe_index(coordinates)] = val;
}

void set_pixel(Point coordinates, uint8_t val){
    uint8_t index = get_pixel_index(coordinates.y);
    insert_bit(index, val, SCREEN + pixel_to_stripe(coordinates));
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



void write_char(Point chcoord, char ch){
	int i;
	int c = ch;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe((Point){chcoord.x + i, chcoord.y}, FONT[c * 8 + i]);
	}
}
void invert_char(Point chcoord){
	int i;
	chcoord = chcoord_to_stcoord(chcoord);
	for(i = 0; i < 8; ++i){
		set_stripe((Point){chcoord.x + i, chcoord.y}, ~get_stripe((Point){chcoord.x + i, chcoord.y}));
	}
}

void write_string(uint8_t line, char const *str){
	line %= 4;
	int i;
	for(i = 0; i < 16; ++i){
		if(*str == '\n' || *str == '\0'){
			for(i; i < 16; ++i) write_char((Point){i, line}, ' ');
			return;
		}
		if(*str & 0x80) continue;
		write_char((Point){i, line}, *str);
		str++;
	}
}
void invert_string(uint8_t line){
	line %= 4;
	int i, j;
	for(i = 0; i < 16; ++i){
		invert_char((Point){i, line});
	}
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

void update_disp(){
    uint8_t i, j;
    for(i = 0; i < STRIPE_ROWS; ++i){
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
		spi_send_recv(i);

        spi_send_recv(0x00);
		spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < STRIPE_COLS; ++j)
			spi_send_recv(get_stripe((Point){j, i}));
    }
}