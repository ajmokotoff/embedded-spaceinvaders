//includes
#include <Alien.h>
#include <msp430.h>
#include <stdint.h>
#include "inc\hw_memmap.h"
#include "driverlibHeaders.h"
#include "CTS_Layer.h"
#include "stdio.h"
#include "stdlib.h"
#include "grlib.h"
#include "LcdDriver/Dogs102x64_UC1701.h"
#include "peripherals.h"
#include <string.h>
// Define global variables
long stringWidth = 0;
Alien aliens[25];
int loop=1;
int state=0;
char level = '1';
int levelNum=3;
Alien emptyAlien;
int counter=0;
int emptyColumns[5];
int levelSpeed = 15;
// Function prototypes for this file
void swDelay(char numLoops);
void welcome();
void checkForStart();
void countDown();
void clearDisplay();
void createLevel();
void addRow();
void writeToDisplay();
void iterate();
void gameOver();
void checkPads();
void clearAlien(int col);
void countDown();
void displayLevel();
void nextLevel();
void victory();
void checkForWin();

//main program
void main(void)
{
	emptyAlien = createAlien("",0,0,0); //empty alien


	// Stop WDT
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //Perform initializations (see peripherals.c)
    configTouchPadLEDs();
    configDisplay();
    configCapButtons();

    while(loop) //main while loop for the game
    {
		P1OUT |= (LED4+LED5+LED6+LED7+LED8);   // turn on all 5 LEDs

	    switch(state) //game states
		{
			case 0: //welcome
				welcome();
				break;
			case 1: //wait for start
				checkForStart(); //check if X was pressed
				break;
			case 2: //count down
				displayLevel(); //display the Level number
				countDown(); //count down
				state++;
				break;
			case 3: //create level
				createLevel(); //create the new level to the array
				writeToDisplay(); //update the display with the game
				state++;
				break;
			case 4: //move aliens and check for shots
				if(!(counter%levelSpeed)) //wait a couple cycles before iterating the game to give time to check for touch pads
				{
					counter=0;
					iterate();
				}
				checkPads(); //check touch pads
				BuzzerOff(); //turn buzzer off
				break;
			case 5:
				nextLevel(); //set up speed for next level
				break;
			default:
				gameOver(); //game over return to main screen
				state = 0;
				break;
		}
    	counter++;
    }
}

void displayLevel() //display level number
{
	char output[9] = {'L','e','v','e','l',':',' ',level,'\0'};
	GrStringDrawCentered(&g_sContext, output, AUTO_STRING_LENGTH, 51, 32, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //update
	swDelay(3); //wait
	clearDisplay(); //clear display
}

void swDelay(char numLoops)
{
	volatile unsigned int i,j;	// volatile to prevent optimization
			                            // by compiler
	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}

void welcome()
{
	// Intro Screen -- Write to the display screen
	GrStringDrawCentered(&g_sContext, "Welcome to", AUTO_STRING_LENGTH, 51, 16, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Space Invaders!", AUTO_STRING_LENGTH, 51, 32, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Press X to Play!", AUTO_STRING_LENGTH, 51, 48, TRANSPARENT_TEXT);

	// Refresh the display now that we have finished writing to it
	GrFlush(&g_sContext);
	state=1;
}

void checkForStart() //check if X was pressed to start
{
	if (CapButtonRead() == 0x01)
	{
		state = 2;
		clearDisplay(); //clear display
	}
}

void clearDisplay()
{
	GrClearDisplay(&g_sContext); //clear display
}

void displayCount(char* print)//display function for count down
{
	GrStringDrawCentered(&g_sContext, print, AUTO_STRING_LENGTH, 51, 32, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //update
	swDelay(1); //wait
	clearDisplay(); //clear display
}
void countDown() //count down on screen before level
{
	displayCount("3.");
	displayCount("3..");
	displayCount("3...");
	displayCount("2.");
	displayCount("2..");
	displayCount("2...");
	displayCount("1.");
	displayCount("1..");
	displayCount("1...");
}

void createLevel() //Create the new level into the game array
{
	int j;
	for(j=0;j<5;j++)
	{
		emptyColumns[j]=1;
	}
	addRow();
	int i;
	for(i=5;i<25;i++)
	{
		aliens[i] = emptyAlien;
	}
}

void addRow() //add a row of aliens to the game
{
	aliens[0] = emptyAlien;
	aliens[1] = emptyAlien;
	aliens[2] = emptyAlien;
	aliens[3] = emptyAlien;
	aliens[4] = emptyAlien;

	int numOfAliens;
	int alienToSpawn;
	int numOfAliensToSpawn = 1 + (rand() % levelNum); //use a random key to randomize how many to spawn

	for(numOfAliens=0; numOfAliens < numOfAliensToSpawn; numOfAliens++) //for the number of aliens, pick random columns
	{
		do{ //make sure there aren't repeats
		alienToSpawn = (rand() % 5);
		} while(aliens[alienToSpawn].xloc);
		switch(alienToSpawn) //add an alien in the right column
		{
			case 0:
				aliens[alienToSpawn] = createAlien("X",10,5,10);
				emptyColumns[alienToSpawn] = 0;
				break;
			case 1:
				aliens[alienToSpawn] = createAlien("N",30,5,10);
				emptyColumns[alienToSpawn] = 0;
				break;
			case 2:
				aliens[alienToSpawn] = createAlien("*",50,5,10);
				emptyColumns[alienToSpawn] = 0;
				break;
			case 3:
				aliens[alienToSpawn] = createAlien("Y",70,5,10);
				emptyColumns[alienToSpawn] = 0;
				break;
			case 4:
				aliens[alienToSpawn] = createAlien("O",90,5,10);
				emptyColumns[alienToSpawn] = 0;
				break;
			case 5:
				nextLevel();
				break;
			default:
				break;
		}
	}
}

void nextLevel() //set game speed to a harder difficulty
{
	swDelay(3);
	level = '2';
	levelNum = 5;
	levelSpeed = 8;
	state=2;
	displayLevel();
}

void writeToDisplay() //update LCD screen with the game
{
	clearDisplay();
	int i;
	for(i=0;i<25;i++)
	{
		if(aliens[i].xloc)
		{
			if(i%2)
			{
				GrCircleDraw(&g_sContext,aliens[i].xloc,aliens[i].yloc,3);
			}
			else
			{
				GrCircleFill(&g_sContext,aliens[i].xloc,aliens[i].yloc,3);
			}
		}
	}
	GrFlush(&g_sContext);
}

void iterate()
{
	checkForWin(); //check to see if there aren't aliens
	int i;
	for(i=24;i>=0;i--) //for loop starting to bottom to check if aliens are at the bottom
	{
		if((aliens[i].xloc > 0)) //check to see if it isn't an empty alien
		{
			if(aliens[i].yloc >= 40) //check if game is over
			{
				state = 6; //game over
				return;
			}
			aliens[i].yloc += 10;
			aliens[i+5] = aliens[i]; //move the alien down
			aliens[i] = emptyAlien; //make it an empty alien
		}
	}
	addRow(); //add a row of new aliens
	writeToDisplay(); //update the LCD screen
}

void gameOver() //game is over return to home screen
{
	clearDisplay();
	GrStringDrawCentered(&g_sContext, "Game Over!!", AUTO_STRING_LENGTH, 51, 32, TRANSPARENT_TEXT);
	GrFlush(&g_sContext);
	swDelay(3);
	clearDisplay();
}

void victory() //you won, return to home screen
{
	clearDisplay();
	GrStringDrawCentered(&g_sContext, "Victory!!", AUTO_STRING_LENGTH, 51, 32, TRANSPARENT_TEXT);
	GrFlush(&g_sContext);
	swDelay(3);
	clearDisplay();
}

void checkPads() //check pads to see if you should clear some aliens
{
	checkForWin();
	CAP_BUTTON keypressed_state = CapButtonRead();
	switch(keypressed_state) //clear the right alien depending on the button pressed
	{
		case 0x01:
			clearAlien(20);
			break;
		case 0x02:
			clearAlien(21);
			break;
		case 0x04:
			clearAlien(22);
			break;
		case 0x08:
			clearAlien(23);
			break;
		case 0x10:
			clearAlien(24);
			break;
		default: //if nothing is pressed
			break;
	}
}

void buzzer(int num) //turn buzzer on
{
	BuzzerOn();
}

void clearAlien(int col) //clear the correct alien depending on the pad pressed
{
	BuzzerOn();
	int i;
	for(i=col; i>=0; i-=5)
	{
		if(aliens[i].xloc > 0)
		{
			aliens[i] = emptyAlien;
			if(i<5)
			{
				emptyColumns[i]=1;
			}
			i=0;
		}
	}
	writeToDisplay(); //update LCD screen
	checkForWin(); //check for win
}

void checkForWin() //check if you won
{
	int j;
	int empty = 0;
	for(j=0;j<5;j++)
	{
		empty += emptyColumns[j];
	}
	if(empty == 5)
	{
		if(levelNum==5)
		{
			state=6;
			level = '1';
			levelNum = 3;
			levelSpeed = 15;
			victory();
			return;
		}
		state = 5;
		return;
	}
}
