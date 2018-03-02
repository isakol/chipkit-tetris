/* This file written 2018 by Alexander Leo and Isak Olsson */

#include <stdbool.h>
#include <stdlib.h>

const int CENTERPIECEx = 20;
const int CENTERPIECEy = 5;


//2 bits = starting location offset from centerpiece for block 1 // 4*3 bits = block 0,2,3 offset from block 1. //4*4 bits = offset for rotation points
//rotation points won't be filled in by the block after rotation, but has to be rotated across when rotating.
//points have 4 bits, first 2 bits represent X. Last 2 represents Y.
//Generally 00 = -1, 01 = 0, 10 = 1. For block 3, and rotationpoints 3 and 4, it's instead (for x) 00=-1 01=0 10=1 11=2 (for y) 00=-2 01=-1 10=0 11=1

														//offs b0   b2   b3   r1   r2   r3   r4
const int JBlock0def = 0b010100011000110001010101100110; //01 0100 0110 0011 0001 0101 0110 0110
const int JBlock1def = 0b011001000100011000010101100110; //01 1001 0001 0001 1000 0101 0110 0110
const int JBlock2def = 0b010110010010011010010101100110; //01 0110 0100 1001 1010 0101 0110 0110
const int JBlock3def = 0b010001100110110010010101100110; //01 0001 1001 1011 0010 0101 0110 0110

const int LBlock0def = 0b010110010000011010010101100110; //01 0110 0100 0001 1010 0101 0110 0110
const int LBlock1def = 0b010001100110010010010101100110; //01 0001 1001 1001 0010 0101 0110 0110
const int LBlock2def = 0b010100011010110000010101100110; //01 0100 0110 1011 0000 0101 0110 0110
const int LBlock3def = 0b011001000100111000010101100110; //01 1001 0001 0011 1000 0101 0110 0110

const int SBlock0def = 0b010110000100011010010101100110; //01 0110 0001 0001 1010 0101 0110 0110
const int SBlock1def = 0b011001011000111010010101100110; //01 1001 0110 0011 1010 0101 0110 0110

const int ZBlock0def = 0b010100000100110000010101100110; //01 0100 0001 0011 0000 0101 0110 0110
const int ZBlock1def = 0b010001011010110000010101100110; //01 0001 0110 1011 0000 0101 0110 0110

const int TBlock0def = 0b000100011000101010001000010110; //00 0100 0110 0010 1010 0010 0001 0110
const int TBlock1def = 0b011001000101011000001000010110; //01 1001 0001 0101 1000 0010 0001 0110
const int TBlock2def = 0b010110010010100000100010110110; //01 0110 0100 1010 0000 1000 1011 0110
const int TBlock3def = 0b010001100101110010100010110110; //01 0001 1001 0111 0010 1000 1011 0110

const int IBlock0def = 0b000110010001000010100011011000; //00 0110 0100 0100 0010 1000 1101 1000
const int IBlock1def = 0b100001100111100010100011011000; //10 0001 1001 1110 0010 1000 1101 1000

const int OBlockdef = 0b000110001000100101010101100110; //00 0110 0010 0010 0101 0101 0110 0110

//create a definition of a block that has an x and a y value
typedef struct {
	int x;
	int y;
}Block;

//create a definition of a shape
typedef struct {
	Block block[4];
	int state[4];
	int currentState; //rotation
}Shape;

//create bool array to contain all the fallen blocks
bool fallenblocks[20][10];

//create object shape
static Shape shape;

int nextshape;

//extern to be able to use score and difficulty declared in other file
extern int score;
extern int difficulty;


void createShape(int random) {

	nextshape = rand()&7; // nextshape is a random number between 0-6 so mask with 7
	
	//we only want 0-6, not 7 so if it's 7, do this loop till it's not
	while (nextshape == 7) {
		nextshape = rand()&7;
	}
	
	//check which shape to create and use respective shape definition integer
	switch(random) {
	case 0:
	shape.state[0] = JBlock0def;
	shape.state[1] = JBlock1def;
	shape.state[2] = JBlock2def;
	shape.state[3] = JBlock3def;
	break;
	
	case 1:
	shape.state[0] = LBlock0def;
	shape.state[1] = LBlock1def;
	shape.state[2] = LBlock2def;
	shape.state[3] = LBlock3def;
	break;
	
	case 2:
	shape.state[0] = SBlock0def;
	shape.state[1] = SBlock1def;
	shape.state[2] = SBlock0def;
	shape.state[3] = SBlock1def;
	break;
	
	case 3:
	shape.state[0] = ZBlock0def;
	shape.state[1] = ZBlock1def;
	shape.state[2] = ZBlock0def;
	shape.state[3] = ZBlock1def;
	break;
	
	case 4:
	shape.state[0] = TBlock0def;
	shape.state[1] = TBlock1def;
	shape.state[2] = TBlock2def;
	shape.state[3] = TBlock3def;
	break;
	
	case 5:
	shape.state[0] = IBlock0def;
	shape.state[1] = IBlock1def;
	shape.state[2] = IBlock0def;
	shape.state[3] = IBlock1def;
	break;
	
	case 6:
	shape.state[0] = OBlockdef;
	shape.state[1] = OBlockdef;
	shape.state[2] = OBlockdef;
	shape.state[3] = OBlockdef;
	break;
	
	default:
	shape.state[0] = IBlock0def;
	shape.state[1] = IBlock1def;
	shape.state[2] = IBlock0def;
	shape.state[3] = IBlock1def;
	
	}
	
	//define what blocks a shape contains
	shape.block[1].x = CENTERPIECEx-(((shape.state[0])>>28)&0b0011);
	shape.block[1].y = CENTERPIECEy;
	shape.block[0].x = (shape.block[1].x)+(((shape.state[0])>>26)&0b0011) - 1;
	shape.block[0].y = (shape.block[1].y)+(((shape.state[0])>>24)&0b0011) - 1;
	shape.block[2].x = (shape.block[1].x)+(((shape.state[0])>>22)&0b0011) - 1;
	shape.block[2].y = (shape.block[1].y)+(((shape.state[0])>>20)&0b0011) - 1;
	shape.block[3].x = (shape.block[1].x)+(((shape.state[0])>>18)&0b0011) - 1;
	shape.block[3].y = (shape.block[1].y)+(((shape.state[0])>>16)&0b0011) - 2;
	
	
	shape.currentState = 0; //set current state of shape
	
		
}

void drawShape(void) {
	int i;
	int j;
	for (i=0;i<4;i++) {
		drawBlock(shape.block[i].x, shape.block[i].y); //loop through each block of a shape and draw them
	}
}

void drawNextShape(void) {
	switch(nextshape) {
		
		//J
		case 0:
		drawBlock(23,6);
		drawBlock(23,5);
		drawBlock(23,7);
		drawBlock(22,7);
		break;
		
		//L
		case 1:
		drawBlock(23,6);
		drawBlock(23,5);
		drawBlock(23,7);
		drawBlock(22,5);
		break;
		
		//S
		case 2:
		drawBlock(22,5);
		drawBlock(22,6);
		drawBlock(23,6);
		drawBlock(23,7);
		break;
		
		//Z
		case 3:
		drawBlock(23,5);
		drawBlock(23,4);
		drawBlock(22,5);
		drawBlock(22,6);
		break;
		
		//T
		case 4:
		drawBlock(23,7);
		drawBlock(22,6);
		drawBlock(23,6);
		drawBlock(23,5);
		break;
		
		//I
		case 5:
		drawBlock(23,5);
		drawBlock(23,4);
		drawBlock(23,6);
		drawBlock(23,7);
		break;
		
		//O
		case 6:
		drawBlock(22,6);
		drawBlock(23,6);
		drawBlock(22,5);
		drawBlock(23,5);
		break;
		
		default:
		drawBlock(23,5);
		drawBlock(23,4);
		drawBlock(23,6);
		drawBlock(23,7);
		
	}
}

//draw the fallen blocks on bottom of gamefield
void drawFallenblocks(void) {
	int x;
	int y;
	for(x = 0; x<20; x++){
		for(y = 0; y<10; y++){
			if(fallenblocks[x][y]){ //if a block exists at the index, draw it
				drawBlock(x+1, y+1);
			}
		}
	}
}

void breakLine(int line){
	int i;
	for(i = 0; i<10; i++){ //loop through the line and remove all blocks from that line
		fallenblocks[line][i] = 0;
	}
	line++;
	for(line; line<20; line++){
		for(i = 0; i<10; i++){
			//if exists
			if(fallenblocks[line][i]){
				//move block downwards.
				fallenblocks[line][i]=0;
				fallenblocks[line-1][i]=1;
			}
		}
	}
	
	score += 10*(difficulty+1); //increment score
}

//remove all blocks from fallenblocks
void clearBlocks(void) {
	int i;
	int j;
	for(i=0;i<20;i++) {
		for (j=0;j<10;j++) {
			if (fallenblocks[i][j]) {
				fallenblocks[i][j] = 0;
			}
		}
	}
}

void checkLines(void){
	int line;
	int count;
	int y;
	for(line = 0; line<20; line++){ //loop through each row on the game field
		count = 0;
		for(y = 0; y<10; y++){
			if(!(fallenblocks[line][y])){ //add count for all empty blocks on the line
				count++;
			}
			
		}
		if(count==0){ //if the count is zero, it means that there are no empty blocks on the line, its full, break line
			breakLine(line);
			line--;
			}
	}
	
}

//check if movement leads to collision.
bool collisionCheck(void){
	int x;
	int y;
	int i;
	
	//loop through each block and check if it exists in fallenblocks
	for(i = 0; i<4; i++){
		x = shape.block[i].x;
		y = shape.block[i].y;
		if(fallenblocks[x-1][y-1]){
		return 1;
		}
	}
	
	//check if any block is 0 (at the floor)
	if (shape.block[0].x == 0 || shape.block[1].x == 0 || shape.block[2].x == 0 || shape.block[3].x == 0) { 
	return 1;
	}
	
	else{
	return 0;
	}
	
}


void impact(void) {
	
	int i=0;
	int j=0;
	int xval;
	int yval;
	int q;
	
	//add shape to fallenblocks
	for(q = 0; q<4; q++){
		xval = shape.block[q].x;
		yval = shape.block[q].y;
		fallenblocks[xval-1][yval-1]=1;
	}
	
	//create a new shape, check if full line
	createShape(nextshape);
	checkLines();
	
	//if a collision is made right after the new shape is created, it means game over
	if (collisionCheck()) {
		clearBlocks();
		gameover();
	} else {
		drawShape();
	}

}

bool wallCheck(void){
	//check against left wall
	if (shape.block[0].y <= 0 || shape.block[1].y <= 0 || shape.block[2].y <= 0 || shape.block[3].y <= 0){
	return 1;
	}
	//check against right wall
	if (shape.block[0].y >= 11 || shape.block[1].y >= 11 || shape.block[2].y >= 11 || shape.block[3].y >= 11){
	return 1;
	}
	return 0;
}

bool rotationCheck(void){
	
	int pointX;
	int pointY;
	int i;
	
	for(i = 0; i<4; i++){
		int yOffset = 1;
		if(i>2){
			yOffset = 2;
		}
		pointX = (shape.block[1].x)+(((shape.state[shape.currentState])>>(14-(i*4)))&0b0011) - 1;
		pointY = (shape.block[1].y)+(((shape.state[shape.currentState])>>(12-(i*4)))&0b0011) - yOffset;
		
		//check for obstructing blocks, or floor, or wall
		//if(fallenblocks[pointX-1][pointY-1] | pointX <= 0 | pointY <= 0 | pointY >= 11){
				
		//check for obstructing blocks	, walls/floor check not necessary?	
		if(fallenblocks[pointX-1][pointY-1] | pointX <= 0 | pointY <= 0 | pointY >= 11){
			return 1;
		}
	}
	return 0;
}

//make block go one step down
void gravity(void) {
	int i;
	for (i=0;i<4;i++) {
		shape.block[i].x -= 1;
	}
	
	//if a collision is detected, reverse the move
	if(collisionCheck()){
		for (i=0;i<4;i++) {
			shape.block[i].x += 1;	
		}
		impact();	
	}
	
}

void moveLeft(void) {
	int i;
	for (i=0;i<4;i++) {
		shape.block[i].y -= 1;	
	}	
	
	//check if trying to pass over wall.
	if(wallCheck()){
			
		//revert movement
		for (i=0;i<4;i++) {
			shape.block[i].y += 1;
		}
			
		//no wall, check for other collision
	}else if(collisionCheck()){
		
		//revert movement
		for (i=0;i<4;i++) {
			shape.block[i].y += 1;
		}
			
	}
		
}

void moveRight(void) {
	int i;
		for (i=0;i<4;i++) {
			shape.block[i].y += 1;
		}
		
		//check if trying to pass over wall.
		if(wallCheck()){
			
			//revert movement
			for (i=0;i<4;i++) {
				shape.block[i].y -= 1;
			}
			
			//no wall, check for other collision
		}else if(collisionCheck()){
			//revert movement
			for (i=0;i<4;i++) {
				shape.block[i].y -= 1;
			}
		}
}

void rotate(void) {
	int i;
	
	//change state of the shape
	if(shape.currentState==3){
		shape.currentState=0;
	}else{
		(shape.currentState)++;
	}
	
	//change every block of the shape (bits taken from shape definition integer)
	shape.block[0].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>26)&0b0011) - 1;
	shape.block[0].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>24)&0b0011) - 1;
	
	shape.block[2].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>22)&0b0011) - 1;
	shape.block[2].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>20)&0b0011) - 1;
	
	shape.block[3].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>18)&0b0011) - 1;
	shape.block[3].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>16)&0b0011) - 2;
	
	
	//check if trying to pass over wall.
	if(wallCheck() | collisionCheck() | rotationCheck()){
			
		//revert movement
		if(shape.currentState==0){
			shape.currentState=3;
		}else{
			(shape.currentState)--;
		}
			
		shape.block[0].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>26)&0b0011) - 1;
		shape.block[0].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>24)&0b0011) - 1;
	
		shape.block[2].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>22)&0b0011) - 1;
		shape.block[2].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>20)&0b0011) - 1;
	
		shape.block[3].x = (shape.block[1].x)+(((shape.state[shape.currentState])>>18)&0b0011) - 1;
		shape.block[3].y = (shape.block[1].y)+(((shape.state[shape.currentState])>>16)&0b0011) - 2;	
		
	}
		
		
		
}