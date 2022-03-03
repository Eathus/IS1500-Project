#include <stdlib.h>
#include <stdio.h>
#include "u32graphics.h"
#include "snakelogic.h"
#include "snakegame.h"
#include "menus.h"
#include "data.h"


int main(void){
    /*
        Setup:
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
    
    game_state current = Start;
    int current_score = 0;
    Score scores[10];
    Sboard score_board = {scores, 0, 10};

    difficulty Level_diff = None;
    difficulty ai_diff = None;

    //Main game loop
    while (1){
        
        /*
        *   Check current every iteration to see what screen to display next
        *   and keep track of state of the program
        */
        switch (current)
        {
        case Start:{
            start_screen();
            current = Main;
            break;
        }
        //Main menu that apears after start screen
        case Main:{
            Options_button main_options[] = {
                //casting to avoid "undefined reference to `memcpy'" - error
                (Options_button){Game, "PLAY\n", 0},
                (Options_button){Score_board, "SCORE BOARD\n", 0},
                (Options_button){Leveld, "LEVEL DIFFICULTY\n", 0},
                (Options_button){AId, "AI DIFFICULTY\n", 0},
                (Options_button){Start, "BACK TO START\n", 0},
                (Options_button){Quit, "QUIT\n", 0}
            };
            Options_menu main_menu = {
                main_options,
                0, 0, 6, 3
            };
            Image menu_help = {
                Row, 128, 8, LOCATOR_MENU_HELP
            };
            draw_image(SCREEN, &menu_help, (Point){0, 0});
            current = locator_menu(&main_menu, BTN3_4, SCREEN);
            break;
        }
        case Game:
            current_score = 0;
            current = snake_game(ai_diff, Level_diff, &current_score);
            break;
        //Options immediately after dying in snake game
        case End_options:{
            Options_button *end_options;
            Options_menu end_menu;
            int score_pos = top_score(&score_board, current_score);
            if(score_pos != -1){
                end_options = (Options_button[]){
                    (Options_button){Name_input, "SAVE SCORE\n", 0},
                    (Options_button){Game, "RETRY\n", 0},
                    (Options_button){Main, "BACK TO MENU\n", 0}
                };
                Options_menu end_menu = {
                    end_options,
                    0, 0, 3, 2
                };
                write_row(SCREEN, 0, "   GAME OVER   ");
                Image menu_help = {
                    Row, 128, 8, LOCATOR_MENU_HELP
                };
                draw_image(SCREEN, &menu_help, (Point){0, 8});
                current = locator_menu(&end_menu, BTN3_4, SCREEN);
            }
            else{
                end_options = (Options_button[]){
                    (Options_button){Game, "RETRY\n", 0},
                    (Options_button){Main, "BACK TO MENU\n", 0}
                };
                Options_menu end_menu = {
                    end_options,
                    0, 0, 2, 2
                };
                write_row(SCREEN, 0, "   GAME OVER   ");
                Image menu_help = {
                   Row, 128, 8, LOCATOR_MENU_HELP
                };
                draw_image(SCREEN, &menu_help, (Point){0, 8});
                current = locator_menu(&end_menu, BTN3_4, SCREEN);  
            }
            break;
        }
        //Keyboard menu for writing your name to save your scores
        case Name_input:{
            Score score;
            score.score = current_score;
            score.name[MAX_NAME_LEN] = 0;
            current = name_input(score.name, SCREEN);
            insert_score(&score_board, &score);
            
            break;
        }
        case Score_board:
            current = score_board_menu(&score_board, SCREEN);
            break;
        //Level difficulty menu
        case Leveld:{ 
            Options_button diffs[] = {
                (Options_button){None, (char[]){"NO OBSTACLES   "}, 0},
                (Options_button){Normal, (char[]){"NORMAL         "}, 0},
                (Options_button){Hard, (char[]){"HARD           "}, 0}
            };
            Options_menu ob_diff_menu = {
                diffs,
                0, 0, 3, 3
            };
            current = diff_menu(&Level_diff, &ob_diff_menu, SCREEN);  
            break;
        }
        //Ai difficulty menu
        case AId:{
            Options_button diffs[] = {
                (Options_button){None, (char[]){"NO AI          "}, 0},
                (Options_button){Easy, (char[]){"EASY           "}, 0},
                (Options_button){Normal, (char[]){"NORMAL         "}, 0},
                (Options_button){Hard, (char[]){"HARD           "}, 0}
            };
            Options_menu ob_diff_menu = {
                diffs,
                0, 0, 4, 3
            };
            current = diff_menu(&ai_diff, &ob_diff_menu, SCREEN);  
            break;
        }
        case Quit:
            clear_frame(SCREEN);
            update_disp(SCREEN);    
            return 0;
            break;
        default:
            break;
        }
    }

    return 0;
}