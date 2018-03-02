/* This file written 2018 by Alexander Leo and Isak Olsson
   from original code written by F Lundevall
   
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h> /* Use of bool */
#include <stdlib.h> /* Use of rand() */



int seed = 0;
int seedplus = 1;
int timeoutcount = 0;
int screen = 0;
int difficulty = 0;
int score = 0;
int highscore = 0;

//declare stdout as a pointer to null to be able to use functions in stdlib
void *stdout = (void *) 0;

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

void init( void )
{
	TRISECLR = 0xff; 
	TRISDSET = 0xfe0; // TRISDSET 
	T2CON = 0x70; //1/256 prescale clears bit 15, on bit.
	PR2 = (80000000/256)/10; //timeout period 0,1s
	TMR2 = 0;	//reset timer
	T2CONSET = 0x8000; // Timer configured. Time to start the timer
  return;
}


void gameover(void) {
	timeoutcount++;
	
	//change to screen 2 (gamover) so that the game will stay here
	screen = 2;
	
	//display GAME OVER text
	display_string(0, "GAME OVER!");
	display_string(1, "POINTS: ");
	display_string(2, itoaconv(score));
	
	//check if highscore and print "NEW HIGH SCORE"
	if (score>highscore) {
		display_string(3, "NEW HIGH SCORE!");
	} else {
		display_string(3, "");
	}
	
	display_update();
	
	//stay on game over screen for 5 seconds then change back to main menu
	if (timeoutcount==50) {
		//set high score, then reset the score
		if (score>highscore) {
		highscore = score;
		}
		score = 0;
		timeoutcount = 0;
		
		//go back to main menu
		screen=0;
	}
}


void game(void) {
		timeoutcount++;
		
		//make shape go down one block every 16 ticks by default
		//if one or more switches are active, the difficulty will be higher and the shapes will fall faster
		if (timeoutcount == 16-difficulty) {
			gravity();
			timeoutcount=0;
		}
	
		if(getbtns()>0){
			
		//rotate piece if holding BTN1
		if ((getbtns() & 0b1000) == 0b1000) {
			rotate();
		}
		
		//move piece right if holding BTN2
		if((getbtns() & 0b001) == 0b001){
			moveRight();
		}
		//make piece fall faster if holding BTN3
		if((getbtns() & 0b010) == 0b010){
			gravity();
		}
		
		//move piece left if holding BTN4
		if((getbtns() & 0b100) == 0b100){
			moveLeft();
		}
		
		
	}
	
		//draw game elements
		drawBorder();
		drawShape();
		drawNextShape();
		drawFallenblocks();
		drawGameText();
		
		int scorelength = intLength(score);
		drawScore(scorelength, score);
		
		int difflength = intLength(difficulty);
		drawDiff(difflength, difficulty);
		
		display_image();
}

void highscores(void) {
	
		display_string(0, "HIGH SCORE:");
		display_string(1, itoaconv(highscore));
		display_string(2, "");
		display_string(3, "BTN4: BACK");
		display_update();
		
		//BTN4 (go back to main menu
		if((getbtns() & 0b100) == 0b100) {
			screen=0;
		}
}

void menu(void) {

		//BTN4 (go to game)
		if((getbtns() & 0b100) == 0b100) {
			
			//make sure we only seed srand once
			if (seedplus == 1) {
			seedplus = 0;
			srand(seed);
			}
			
			//get difficulty from switches (reads in binary)
			difficulty = getsw();
			
			//create empty shape to be used as the active falling piece
			int randomstartshape = rand()&7;
			while (randomstartshape == 7) {
				randomstartshape = rand()&7;
			}
			createShape(randomstartshape);
			
			screen=1; //go to game
		}
		
		//BTN3 (go to highscores)
		if((getbtns() & 0b010) == 0b010){
			screen=3; //go to highscores
		}
		
		//display menu options
		display_string(0, "TETRIS");
		display_string(1, "");
		display_string(2, "BTN4: PLAY");
		display_string(3, "BTN3: HIGH SCORE");
		display_update();
}

/* This function is called repetitively from the main program */
void work( void )
{
	//while user has not presed a button, increment the seed
	//this is placed outside the interrupt flag check to make it increment faster, creating a more random seed
	if (seedplus) {
	seed++;
	//make sure the seed doesnt get too big, start it from zero again
	if (seed > 100000000)
		seed = 0;
	}
	
	if(IFS(0) & 0x100){	//test timeout event flag
	IFSCLR(0) = 0x100;	//clear timeout flag
	switch (screen) {
		case 0:
		menu();
		break;
		case 1:
		game();
		break;
		case 2:
		gameover();
		break;
		case 3:
		highscores();
		break;
	}
  }
  
}
