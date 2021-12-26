#include "u32graphics.h"
#include <stdio.h>

uint8_t SCREEN[128*4];
uint16_t SNAKES[128*4];

int main(void){
    /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
    init_disp();
    uint8_t i;
    for( i = 0; i < 15; ++i){
        set_pixel((Point){i, 16}, 1);
    }
    
    for( i = 0; i < 15; ++i){
        set_pixel((Point){i, 17}, 1);
    }
    set_pixel((Point){50, 16}, 1);
    set_pixel((Point){51, 16}, 1);
    set_pixel((Point){50, 17}, 1);
    set_pixel((Point){51, 17}, 1);
    SNAKES;

    update_disp();
    return 0;
}

/*
int main(void) {

    printf("\n");
    //init_disp();
    uint8_t i, j;
    for( i = 0; i < 10; ++i){
        set_pixel((Point){i, 16}, 1);
    }
    for(i = 0; i < 4; ++i){
        for(j = 0; j < 128; ++j){
            printf("%X", get_stripe((Point){j, i}));
        }
        printf("\n");
    }
    printf("\n");
    for(i = 0; i < 4; ++i){
        for(j = 0; j < 128; ++j){
            printf("%X"[4 * j + i]);
        }
        printf("\n");
    }
    printf("\n");
    int f; 
    for(f = 0; f < 128*4; ++f){printf("%X"[f]);}
    printf("\n");
    //update_disp();
}
*/