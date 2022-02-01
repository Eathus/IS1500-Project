#include <stdlib.h>
#include <stdio.h>
#include "u32graphics.h"
#include "snakelogic.h"
#include "snakegame.h"
#include "menus.h"


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
    game_init();
    
    game_state current = Main;
    int current_score = 0;
    Score scores[10];
    Sboard score_board = {scores, 0, 10};

/*
    Score score1;
    score1.score = 2;
    score1.name[0] = 'a';
    score1.name[1] = 0;
    
    Score score2;
    score2.score = 4;
    score2.name[0] = 'b';
    score2.name[1] = 0;

    Score score3;
    score3.score = 5;
    score3.name[0] = 'c';
    score3.name[1] = 0;

    Score score4;
    score4.score = 6;
    score4.name[0] = 'd';
    score4.name[1] = 0;


    Score score5;
    score5.score = 3;
    score5.name[0] = 'e';
    score5.name[1] = 0;

    insert_score(&score_board, &score1);
    insert_score(&score_board, &score2);
    insert_score(&score_board, &score3);
    insert_score(&score_board, &score4);
    insert_score(&score_board, &score5);
*/
    difficulty Level_diff = None;
    difficulty AI_diff = None;
    int cs = 0;
    ai_snake_game(None, &cs);
    return 0;

    while (1){
        int len;
        char* row_num = itoaconv(current, &len);
        write_char((Point){15, 0}, row_num[0]);
        update_disp(); 
        switch (current)
        {
        case Start:{
            //Put start screen code here
            break;
        }
        case Main:{
            Options_button main_options[] = {
                //casting to avoid "undefined reference to `memcpy'" - error
                (Options_button){Game, "Play\n"},
                (Options_button){Score_board, "Score board\n"},
                (Options_button){Level_diff, "Level difficulty\n"},
                (Options_button){AI_diff, "AI difficulty\n"},
                (Options_button){Start, "Back to start\n"},
                (Options_button){Quit, "Quit\n"}
            };
            Options_menu main_menu = {
                main_options,
                0, 0, 6, 4
            };
            current = locator_menu(&main_menu);
            break;
        }
        case Game:
            current_score = 0;
            if(AI_diff == None) current = solo_snake_game(Level_diff, &current_score);
            else; //ai_snake_game();
            break;
        case End_options:{
            Options_button *end_options;
            Options_menu end_menu;
            int score_pos = top_score(&score_board, current_score);
            if(score_pos != -1){
                end_options = (Options_button[]){
                    (Options_button){Name_input, "SAVE SCORE\n"},
                    (Options_button){Game, "RETRY\n"},
                    (Options_button){Main, "BACK TO MENU\n"}
                };
                Options_menu end_menu = {
                    end_options,
                    0, 0, 3, 3
                };
                write_row(0, "GAME OVER");
                current = locator_menu(&end_menu);
            }
            else{
                end_options = (Options_button[]){
                    (Options_button){Game, "RETRY\n"},
                    (Options_button){Main, "BACK TO MENU\n"}
                };
                Options_menu end_menu = {
                    end_options,
                    0, 0, 2, 3
                };
                write_row(0, "GAME OVER");
                current = locator_menu(&end_menu);  
            }
            break;
        }
        case Name_input:{
            //char name[MAX_NAME_LEN + 1];
            Score score;
            score.score = current_score;
            score.name[MAX_NAME_LEN] = 0;
            current = name_input(score.name);
            /*while (1)
            {
                write_row(1, name);
                update_disp(); 
            }*/
            //char displays[16];
            //Score score = {(char *)name, current_score};
            insert_score(&score_board, &score);
            
            break;
        }
        case Score_board:
            current = Score_board_menu(&score_board);
            break;
        case Leveld: 
            
            break;
        case AId:
            
            break;
        case Quit:
            clear_screen();
            return 0;
            break;
        default:
            break;
        }
    }

    /*
    int i;
    int a = 32;
    int b = 23;
    int x = a + b;
    char ch[16];
    for(i = 0; i < 16; ++i) ch[i] = ' ';
    char ch1;
    char *chr = itoaconv(x);  
    for(i = 0; chr[i] != '\0'; ++i) ch[13 + i] = chr[i];
    ch1 = chr[3 ] == '\0' ? 'n' : 'i';
    write_row(0, ch);
    update_disp();
    */



    /*uint8_t i;
    for( i = 0; i < 15; ++i){
        set_pixel((Point){i, 16}, 1);
    }
    
    for( i = 0; i < 15; ++i){
        set_pixel((Point){i, 17}, 1);
    }
    set_pixel((Point){50, 16}, 1);
    set_pixel((Point){51, 16}, 1);
    set_pixel((Point){50, 17}, 1);
    set_pixel((Point){51, 17}, 1);*/
    /*uint16_t snakes[128*4];
    Point head_player[SEGMENT_SIZE];
    Point tail_player[SEGMENT_SIZE];

    Point head_ai[SEGMENT_SIZE];
    Point tail_ai[SEGMENT_SIZE];
    tail_player[0] = (Point){0, 31};
    Point food_pos = {63, 15};
    spawn_snake(tail_player, head_player, 10, Right, snakes);
    game_init();
    //while (1)
    //{
        solo_snake_game(tail_player, head_player, snakes, &food_pos);
    //}*/
    /*
    write_row(1, "abcdefghijk CAPS");
    write_row(1, "abcdefghijk CAPS");
    write_row(2, "lmnopqrstuv BACK");
    write_row(3, "wxyz01234 	SPACE");
    //invert_char((Point){2, 0});
    //invert_row(3);
    update_disp();
    
    
    /*char name[10];
    game_state gs = name_input(name);*/
    
    /*
    Options_button main_options[] = {
                //casting to avoid "undefined reference to `memcpy'" - error
                (Options_button){Game, "Play\n"},
            };
    Options_menu main_menu = {
        main_options,
        0, 0, 1, 3
    };
    locator_menu(&main_menu);
    */
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