#include "u32graphics.h"

#define STRIPE_COLS 128
#define STRIPE_BIT_LEN 8
#define STRIPE_ROWS 4

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