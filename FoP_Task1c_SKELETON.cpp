//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Adrian Chudzikiewicz
//Last updated: 30th April 2018
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for Seed, Random
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.
#include "TimeUtils.h"		//for Time and Date
#include "Data.h"			//for sorting.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------


//defining the size of the grid
const int  SIZEX(29);    	//horizontal dimension
const int  SIZEY(15);		//vertical dimension
//defining symbols used for display of the grid and content
const char SPOT('@');   	//spot
const char SPOTprotected('X');		//spot during moves...
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char HOLE('0');		//hole.
const char PILL('*');		//pill
const char ZOMBIE('Z');		//zombie
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char FREEZE('F');		//to freeze zombies
const char EXTERMINATE('X');//to kill zombies
const char EAT('E');		//to eat all the pills
const char REPLAY('R');		//REPLAY system
const char HIGHSCORE('T');	//to view highscore table
//defining limits
const int playerPreviousScore = -1; //previous score, variable. Used for new players
//global bool for drawing status
bool draw = false;

struct Item {
	int x, y;
	char symbol;
};
struct gameData {
	int numberOfHoles, numberOfLives, numberOfPowerPills, numberOfProtectedMoves, numberOfZombies; //declaration of what will be there.
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	string startScreen();
	int levelSelection();
	void spawnZombies(char grid[][SIZEX], vector<Item>& zombies);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, int& lives, int& pillCount, vector<Item>& zombies, const int& pillLimit, const int& holeLimit);
	void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string username, string previousBestScore, int& lives, int& pillCount, vector<Item>& zombies, int& moveProtected, bool highScoresView);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	/* ZOMBIE KEYS*/
	bool Freeze(const int key); //freezing zombies.
	bool Exterminate(const int key); //killing all visible zombies.
	bool Eat(const int key); //used for eating pills	
	/*END OF ZOMBIE KEYS*/
	bool Replay(const int key);
	bool Highscores(const int key);
	int  getKeyPress();
	int _getKeyPress();
	void updateGameData(char g[][SIZEX], char maze[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, int& lives, int& pillCount, Item& pill, vector<Item>& zombies, bool zombieFrozen, string previousBestScore, string username, bool playerCheated, int& protectedMoves, int& moveProtected, int& levelScore);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item spot, vector<Item>& zombies);
	void eatPills(int& pillCount, char maze[][SIZEX]); //cheat
	/* TEXT FILE FUNCTIONS*/
	void saveScore(string username, int& lives, string previousBestScore, int& levelScore);
	string previousScore(string username);
	void resetFiles();
	void replaySystem(vector<int> keys);
	/* END OF TEXT FILE FUNCTIONS*/
	void showHighscores();
	int measureTime();
	void mainMenu();
	void endProgram(string quitReason, int& lives, string previousBestScore, string username, bool playerCheated, int& levelScore);

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	resetFiles(); //resetting previous files on start. 

	int levelScore = 0;

	string username;
	int level;

	bool firstRun = false;
	if (firstRun == false)
	{
		username = startScreen();
		level = levelSelection();
		mainMenu();
		firstRun = true;
	}
	string previousBestScore = previousScore(username);
	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	Item pill = { PILL };
	Item zombie = { 0, 0, ZOMBIE };

	vector<Item> zombies; // Vector for zombies

	string message("LET'S START...");	//current message to player

	string spotMess("");
	string zombieMess("");

	string quitReason("");

	/*		DEFINITION OF LIMITS		*/
	int lives = 3;
	int pillLimit = 8;
	int pillCount = pillLimit;
	int holeLimit = 12;
	int protectedMoves = 0; //limit on HOW MUCH TO ADD.
	int moveProtected = 0; //this applies to the movement of the spot
	int zombieLimit = 4;  // limit for zombies. RAAWR.
	/* END DEFINITION OF LIMITS*/

	/*************************************/
	/*			LEVEL SETTINGS			 */
	/*************************************/
	//level1
	gameData level1; //slightly harder
	level1.numberOfHoles = holeLimit;
	level1.numberOfLives = lives;
	level1.numberOfPowerPills = pillLimit;
	level1.numberOfProtectedMoves = 10;
	level1.numberOfZombies = zombieLimit;
	//level2
	gameData level2; //slightly harder
	level2.numberOfHoles = holeLimit + 3;
	level2.numberOfLives = lives - 1;
	level2.numberOfPowerPills = pillLimit - 3;
	level2.numberOfProtectedMoves = 5;
	level2.numberOfZombies = zombieLimit;
	//level3
	gameData level3; // a bit harder than previous
	level3.numberOfHoles = holeLimit + 6;
	level3.numberOfLives = lives - 1;
	level3.numberOfPowerPills = pillLimit - 5;
	level3.numberOfProtectedMoves = 3;
	level3.numberOfZombies = zombieLimit;
	//level 4 - CODENAME: IMPOSSIBLE
	gameData level4; // impossible
	level4.numberOfHoles = holeLimit + 9;
	level4.numberOfLives = lives;
	level4.numberOfPowerPills = pillLimit - 7;
	level4.numberOfProtectedMoves = 2;
	level4.numberOfZombies = zombieLimit;

	gameData currentLevel;
	if (level == 1)
		currentLevel = level1;
	else if (level == 2)
		currentLevel = level2;
	else if (level == 3)
		currentLevel = level3;
	else if (level == 4)
		currentLevel = level4;
	/*************************************/
	/*	     END OF LEVEL SETTINGS		 */
	/*************************************/

	//Depending on level selected, readjust objects.
	pillLimit = currentLevel.numberOfPowerPills;
	pillCount = pillLimit; //reassigning it to update visuals.
	holeLimit = currentLevel.numberOfHoles;
	lives = currentLevel.numberOfLives;
	protectedMoves = currentLevel.numberOfProtectedMoves;

	bool zombieFrozen = false; //setting it to false by default.
	bool zombiesKilled = false;
	bool eatAllPills = false;
	bool playerCheated = false;
	bool highscoresView = false;

	bool keyWasPressed = false;

	//action...
	Seed();								//seed the random number generator
	SetConsoleTitle("Spot and Zombies Game - FoP 2017-18");
	initialiseGame(grid, maze, spot, lives, pillCount, zombies, pillLimit, holeLimit);	//initialise grid (incl. walls and spot)
	spawnZombies(grid, zombies);
	paintGame(grid, message, spotMess, zombieMess, username, previousBestScore, lives, pillCount, zombies, moveProtected, highscoresView);			//display game info, modified grid and messages

	int key;							//current key selected by player
	do {		
		if (zombies.size() == 0 && pillCount == 0)
		{
			level++; //increase level if conditions above are met
			if (level == 1) //check what level it's, then transfer user.
				currentLevel = level1;
			else if (level == 2)
				currentLevel = level2;
			else if (level == 3)
				currentLevel = level3;
			else if (level == 4)
				currentLevel = level4;

			levelScore += 100 + lives; //adding 100 points... for completing the level. 

			if (level == 5) //level 4 is playable, 5 isn't. 
			{
				string quitReason = "GameFinished";
				endProgram(quitReason, lives, previousBestScore, username, playerCheated, levelScore);
			}

			pillLimit = currentLevel.numberOfPowerPills;
			pillCount = pillLimit; //reassigning it to update visuals.
			holeLimit = currentLevel.numberOfHoles;
			lives = currentLevel.numberOfLives;
			protectedMoves = currentLevel.numberOfProtectedMoves;
			spawnZombies(grid, zombies);

			zombiesKilled = false; //reupdating zombie kill status. As if it won't be updated, it will save the current state of previous action whic means
								   // it will respawn the zombies.
			eatAllPills = false;

			initialiseGame(grid, maze, spot, lives, pillCount, zombies, pillLimit, holeLimit);
		}
		key = _getKeyPress(); 	//read in  selected key: arrow or letter command

		if (key == UP || key == DOWN || key == RIGHT || key == LEFT || key == EXTERMINATE || key == EAT || key == FREEZE) // if any of these keys are pressed which initiate the game...
			keyWasPressed = true; //make it true, so the timer knows that key was pressed.
		if (keyWasPressed)
		{
			measureTime(); //start measuring the time when key was pressed.
		}
		if (Eat(key))
		{
			eatAllPills = !eatAllPills;
			if (eatAllPills == true)
			{
				playerCheated = true;
				eatPills(pillCount, maze); //eating all pills, without giving the score.
				updateGrid(grid, maze, spot, zombies); //update the gird
				draw = true; //change draw to true, to update the grid. 
			}
		}
		else if (Highscores(key))
		{
			highscoresView = !highscoresView;
			if (highscoresView == true)
			{
				showHighscores();
			}
		}
		else if (Exterminate(key))
		{
			zombiesKilled = !zombiesKilled; //opposite
			if (zombiesKilled == true) // if it's equal to true.
			{
				playerCheated = true;
				zombies.clear(); //clear the vector, delete everything that's inside it.
				updateGrid(grid, maze, spot, zombies); //update the gird
				draw = true; //true to update the grid.
			}
			else
			{
				playerCheated = true;
				spawnZombies(grid, zombies); // if it's pressed again, respawn it.
				draw = true; //update the grid!
			}
		}
		else if (Freeze(key))
		{
			playerCheated = true;
			zombieFrozen = !zombieFrozen; // setting opposite depending what it was before.
			draw = true; //update it!
		}
		else if (Replay(key))
		{
			//todo
		}
		else if (isArrowKey(key))
		{
			updateGameData(grid, maze, spot, key, message, spotMess, zombieMess, lives, pillCount, pill, zombies, zombieFrozen, previousBestScore, username, playerCheated, protectedMoves, moveProtected, levelScore);		//move spot in that direction
			updateGrid(grid, maze, spot, zombies);					//update grid information			
			draw = true;
		}
		else
			message = "INVALID KEY!";	//set 'Invalid key' message
		if (draw) //if draw is true, update the grid.
		{
			paintGame(grid, message, spotMess, zombieMess, username, previousBestScore, lives, pillCount, zombies, moveProtected, highscoresView);		//display game info, modified grid and messages		
		}
		if (draw) draw = false; //if draw is true, make it false.
	} while (!wantsToQuit(key));		//while user does not want to quit
	if (!playerCheated) //if player did NOT cheat. THIS BIT IS FOR QUIT, CONNECTED WITH QUIT COMMAND
	{
		saveScore(username, lives, previousBestScore, levelScore); //save current score
	}
	endProgram(quitReason, lives, previousBestScore, username, playerCheated, levelScore);						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, int& lives, int& pillCount, vector<Item>& zombies, const int& pillLimit, const int& holeLimit)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX], const int& pillLimit, const int& holeLimit);
	void setSpotInitialCoordinates(Item& spot);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item b, vector<Item>& zombies);


	setInitialMazeStructure(maze, pillLimit, holeLimit);		//initialise maze
	setSpotInitialCoordinates(spot);
	updateGrid(grid, maze, spot, zombies);		//prepare grid
}

void setSpotInitialCoordinates(Item& spot)
{ //set spot coordinates inside the grid at random at beginning of game
//If Spot spawns on the WALL, it changes direction by one.
	if (spot.x == WALL)
	{
		spot.x -= 1;
	}
	if (spot.y == WALL)
	{
		spot.y -= 1;
	}
	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
}

void setInitialMazeStructure(char maze[][SIZEX], const int& pillLimit, const int& holeLimit)
{ //set the position of the walls in the maze
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX]; 	//local array to store the maze structure
	// For loops, creating maze ;)
	for (int row(0); row < SIZEY; ++row) //for each column
	{
		for (int col(0); col < SIZEX; ++col) //for each column
		{
			if ((row == 0) || (row == SIZEY - 1))		//Top and Bottom
				initialMaze[row][col] = WALL;			//Draw WALL symbol
			else
				if ((col == 0) || (col == SIZEX - 1))	//Left and Right
					initialMaze[row][col] = WALL;		//Draw WALL symbol
				else
					initialMaze[row][col] = TUNNEL;		//Draw TUNNEL symbol
		}
	}

	//with '#' for wall, ' ' for tunnel, etc. 
	//copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				//not a direct copy, in case the symbols used change
			case '#': maze[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; break;
			}


	//Holes
	for (int i = 0; i < holeLimit; i++)
	{
		int randX = Random(SIZEX - 1); // Random value for X
		int randY = Random(SIZEY - 1); // Random value for Y
		while (maze[randX][randY] != TUNNEL)//make random X and Y within the TUNNEL range.
		{
			randX = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
			randY = Random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
		}
		while (maze[randX - 2][randY - 2] == HOLE)
		{
			randX = Random(SIZEY - 2);
			randY = Random(SIZEX - 2);
		}
		while (maze[randX - 2][randY - 2] == ZOMBIE)
		{
			randX = Random(SIZEY - 2);
			randY = Random(SIZEX - 2);
		}
		maze[randX][randY] = HOLE; //If everything works, insert HOLE there.
	}


	//Pills
	for (int i = 0; i < pillLimit; i++)
	{
		int randX = Random(SIZEX - 1);
		int randY = Random(SIZEY - 1);
		while (maze[randX][randY] != TUNNEL)
		{
			randX = Random(SIZEX - 2);
			randY = Random(SIZEY - 2);
			while (maze[randX][randY] == HOLE)
			{
				randX = Random(SIZEX - 2);
				randY = Random(SIZEY - 2);
			}
			while (maze[randX][randY] == ZOMBIE)
			{
				randX = Random(SIZEX - 2);
				randY = Random(SIZEY - 2);
			}
		}
		maze[randX][randY] = PILL;
	}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const Item spot, vector<Item>& zombies)
{ //update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item spot);

	setMaze(grid, maze);	//reset the empty maze configuration into grid
	for (int zn(0); zn < zombies.size(); ++zn)
	{
		placeItem(grid, zombies[zn]);     //set zombie in grid
	}
	placeItem(grid, spot);	//set spot in grid	
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item item)
{
	g[item.y][item.x] = item.symbol;
}

//---------------------------------------------------------------------------
//----- move items on the grid
//---------------------------------------------------------------------------
void updateGameData(/*const*/char g[][SIZEX], char maze[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, int& lives, int& pillCount, Item& pill, vector<Item>& zombies, bool zombieFrozen, string previousBestScore, string username, bool playerCheated, int& protectedMoves, int& moveProtected, int& levelScore)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void moveZombies(char grid[][SIZEX], int& lives, vector<Item>& zombies, Item& spot, string& zombieMess, int& moveProtected);
	void removePill(char maze[][SIZEX], Item& spot, int& protectedMoves, int& moveProtected);
	void removeMoveProtected(int& moveProtected, Item& spot);
	void endProgram(string quitReason, int& lives, string previousBestScore, string username, bool playerCheated, int& levelScore);

	assert(isArrowKey(key));

	//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	if (zombieFrozen == false)
	{
		moveZombies(g, lives, zombies, spot, zombieMess, moveProtected);
	}

	if (lives <= 0)
	{
		mess = "YOU LOST! PRESS ANY KEY TO QUIT";
		string quitReason = "OutOfLives";
		endProgram(quitReason, lives, previousBestScore, username, playerCheated, levelScore);
	}
	size_t zombieLeft = 0; // comparing vector to ZombieLeft
	if (pillCount == 0 && zombies.size() == zombieLeft)
	{
		mess = "YOU WON GAME WITH " + to_string(lives) + " lives left";
		string quitReason = "pillsEaten";
		endProgram(quitReason, lives, previousBestScore, username, playerCheated, levelScore);
	}
	else {
		//check new target position in grid and update game data (incl. spot coordinates) if move is possible
		switch (g[spot.y + dy][spot.x + dx])
		{			//...depending on what's on the target position in grid...
		case TUNNEL:		//can move
			spot.y += dy;	//go in that Y direction
			spot.x += dx;	//go in that X direction
			removeMoveProtected(moveProtected, spot); //removing 1 move if it's positive.
			break;
		case WALL:  		//hit a wall and stay there, and remove one move
			spotMess = "CANNOT GO THERE!";
			removeMoveProtected(moveProtected, spot);
			break;
		case HOLE:
			lives = lives - 1;
			spotMess = "YOU FELL INTO A HOLE... REMAINNG LIVES:" + to_string(lives) + "	";
			spot.y += dy;
			spot.x += dx;
			removeMoveProtected(moveProtected, spot);
			break;
		case PILL:
			lives = lives + 1;
			spot.y += dy;
			spot.x += dx;
			--pillCount;
			removePill(maze, spot, protectedMoves, moveProtected);
			spotMess = "YOU ATE A PILL... PILLS REMAINING: " + to_string(pillCount) + "	";
			removeMoveProtected(moveProtected, spot);
			break;
		case ZOMBIE:
			lives = lives - 1;
			spot.y += dy;
			spot.x += dx;
			spotMess = "Ahrw, you got hit by zombie";
			zombieMess = "Zombie hit spot";
			removeMoveProtected(moveProtected, spot);
			break;
		}
	}
}
//---------------------------------------------------------------------------
//----- Remove 1 move if positive.
//---------------------------------------------------------------------------
void removeMoveProtected(int& moveProtected, Item& spot)
{
	if (moveProtected == 0)
	{
		//do nothing
	}
	else
	{
		moveProtected--;
	}
}
//---------------------------------------------------------------------------
//----- End of Remove 1 move if positive.
//---------------------------------------------------------------------------


/* TIME STARTS HERE*/
int measureTime()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	static auto start_time = chrono::high_resolution_clock::now();
	static auto prevSecond = start_time; //store start point 
	auto current_time = chrono::high_resolution_clock::now();

	int finalTime = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count(); //use milliseconds to more accurately store time

	if (chrono::duration_cast<chrono::milliseconds>(current_time - prevSecond).count() >= 1000) //if we have a difference of more than 1000 milliseconds, 1 second has elapsed since the previous update
	{
		prevSecond = current_time; //store this time for the next call
		showMessage(clDarkGrey, clYellow, 55, 17, "TIME ELPASED: " + to_string((finalTime / 1000)) + "    seconds");

	}
	return finalTime;
}
/*TIME ENDS HERE*/


//---------------------------------------------------------------------------
//----- Remove Pill
//---------------------------------------------------------------------------
void removePill(char maze[][SIZEX], Item& spot, int& protectedMoves, int& moveProtected) // If Spot coordinates collide with Pills' coordinates.
{
	if (maze[spot.y][spot.x] == PILL)
	{
		moveProtected += protectedMoves;
		maze[spot.y][spot.x] = TUNNEL;
	}
}
void eatPills(int& pillCount, char maze[][SIZEX])
{
	pillCount = 0;
	for (int row = 0; row < SIZEY; ++row)
		for (int col = 0; col < SIZEX; ++col)
		{
			{
				if (maze[row][col] == PILL)
					maze[row][col] = TUNNEL;
			}
		}
}
//---------------------------------------------------------------------------
//----- End of Remove Pill
//---------------------------------------------------------------------------

// Zombies, spawning them. The Big Bang theory of virtual creation of zombies.
void spawnZombies(char grid[][SIZEX], vector<Item>& zombies)
{
	//Zombies
	//initialise

	zombies.push_back({ 1, 1, ZOMBIE });
	zombies.push_back({ SIZEX - 2, 1, ZOMBIE });
	zombies.push_back({ 1, SIZEY - 2, ZOMBIE });
	zombies.push_back({ SIZEX - 2, SIZEY - 2, ZOMBIE });
	for (auto& zombie : zombies) {				//throwing vector of zombies, into grid.
		grid[zombie.y][zombie.x] = ZOMBIE;
	}
}

// Zombies, including virtual kinetic energy which would create kinesiology
void moveZombies(char grid[][SIZEX], int& lives, vector<Item>& zombies, Item& spot, string& zombieMess, int& moveProtected)
{
	int predictedX(0);
	int predictedY(0);
	void resetZombiePosition(vector<Item>& zombies, int arrayIndex); //Will reset the zombies position 
	zombieMess = "					"; //reset msg
	for (int i = 0; i < zombies.size(); ++i) {
		// for each iteration, move zombies towards the player.
		if (moveProtected == 0)
		{
			if (zombies[i].x < spot.x)
			{
				predictedX = zombies[i].x++;
			}
			else if (zombies[i].x > spot.x)
			{
				predictedX = zombies[i].x--;
			}
			if (zombies[i].y < spot.y)
			{
				predictedY = zombies[i].y++;
			}
			else if (zombies[i].y > spot.y)
			{
				predictedY = zombies[i].y--;
			}
		}
		else
		{
			if (zombies[i].x < spot.x)
			{
				predictedX = zombies[i].x--; //inverting move direction
			}
			else if (zombies[i].x > spot.x)
			{
				predictedX = zombies[i].x++;
			}
			if (zombies[i].y < spot.y)
			{
				predictedY = zombies[i].y--; //a little twist...
			}
			else if (zombies[i].y > spot.y)
			{
				predictedY = zombies[i].y++;
			}
		}



		for (int b = 0; b < zombies.size(); ++b)
		{
			if (predictedX != TUNNEL || predictedX != HOLE || predictedX != PILL || predictedY != TUNNEL || predictedY != HOLE || predictedY != PILL)
			{
				for (int j = 0; j < zombies.size(); ++j)
				{
					if (i != j)
					{
						if (predictedX + 1 == zombies[j].x && predictedY + 1 == zombies[j].y)
						{
							zombieMess = "ZOMBIE BUMPED INTO EACH OTHER"; //show what happened on the screen...
							resetZombiePosition(zombies, j); //reset zombie at this position
							resetZombiePosition(zombies, j); //reset zombie at this position
						}
						if (zombies[j].x + 1 == zombies[j].x && zombies[j].y + 1 == zombies[j].y)
						{
							zombieMess = "ZOMBIE BUMPED INTO EACH OTHER";
							resetZombiePosition(zombies, j);
						}
						if (zombies[j].x - 1 == zombies[j].x && zombies[j].y - 1 == zombies[j].y)
						{
							zombieMess = "ZOMBIE BUMPED INTO EACH OTHER";
							resetZombiePosition(zombies, j);
						}
					}
				}
			}
			if (grid[zombies[b].y][zombies[b].x] == HOLE) //if zombie is equal to hole
			{
				if (b == zombies.size() - 1)
				{
					zombieMess = "ZOMBIE FELT INTO THE HOLE";
					zombies.pop_back();
					return;
				}
				zombies.erase(zombies.begin() + b);
			}
			if (grid[zombies[b].y][zombies[b].x] == SPOT) //if zombie is equal to spot
			{
				--lives; //remove life
				resetZombiePosition(zombies, b); //reset zombie
			}
			if (grid[zombies[b].y][zombies[b].x] == WALL) //if zombie is equal to spot
			{
				resetZombiePosition(zombies, b); //reset zombie
			}
		}
		grid[zombies[i].y][zombies[i].x] = ZOMBIE;
	}
}


void resetZombiePosition(vector<Item>& zombies, int arrayIndex)
{
	switch (arrayIndex)
	{
	case 0://top left
		zombies[arrayIndex].x = 1;
		zombies[arrayIndex].y = 1;
		break;
	case 1://bottom left
		zombies[arrayIndex].x = (SIZEX - 2);
		zombies[arrayIndex].y = 1;
		break;
	case 2://top right
		zombies[arrayIndex].x = 1;
		zombies[arrayIndex].y = (SIZEY - 2);
		break;
	case 3://bottom right
		zombies[arrayIndex].x = (SIZEX - 2);
		zombies[arrayIndex].y = (SIZEY - 2);
		break;
	}
}
//---------------------------------------------------------------------------
//----- Text File Functions
//---------------------------------------------------------------------------
void saveScore(string username, int& lives, string previousBestScore, int& levelScore)
{
	int measureTime();
	const int finalScore = lives + levelScore;

	const int finalTime = measureTime()/1000;

	if (previousBestScore == to_string(-1)) //that means that the player is new + converting -1 to string to make comparison string vs string.
	{
		ofstream scoreFile(username + ".txt"); //creating new file		
		scoreFile << finalScore << endl; //adding score to it.
		scoreFile.close(); //closing file.

		/* HIGHSCORE SAVING*/
		ofstream highscoreFile("bestScores.txt", std::ios_base::app | std::ios_base::out); //opening file in APPEND mode so we do not delete previous lines...
		string highscoreFormat = " score was obtained by " + username + " " + "| Date Score was obtained: " + GetDate() + " at " + GetTime() + "| Time: ";
		highscoreFile << finalScore << highscoreFormat << finalTime << " seconds" << endl;
		highscoreFile.close();
	}
	else
	{
		ifstream getPreviousScore; //declaration
		getPreviousScore.open(username + string(".txt")); //accessing file.
		if (getPreviousScore.is_open()) //if everything is okay, proceed
		{
			string previousScore; //declaring string
			getline(getPreviousScore, previousScore); //getting firstline from file getPreviousScore to previousScore string
			int prevScore = stoi(previousScore); //converting string to an int.
			if (prevScore < finalScore) //if finalScore are higher than prevScore
			{
				ofstream scoreFile(username + ".txt"); //open file
				if (scoreFile.is_open()) //if it's opened
				{
					scoreFile << finalScore << endl; //adding score to it.
					scoreFile.close(); //closing file.
					//HIGHSCORE
					ofstream highscoreFile("bestScores.txt", std::ios_base::app | std::ios_base::out); //opening file in APPEND mode so we do not delete previous lines...
					string highscoreFormat = " score was obtained by " + username + " " + "| Date Score was obtained: " + GetDate() + " at " + GetTime() + "| Time: ";
					highscoreFile << finalScore << highscoreFormat << finalTime << " seconds" << endl;
					highscoreFile.close();
				}
			}
			getPreviousScore.close(); //close file
		}
	}

}

string previousScore(string username)
{
	fstream previousScoreFile;
	previousScoreFile.open(username + ".txt"); //opening file with that username.
	if (previousScoreFile.is_open()) //if everything is okay, proceed
	{
		string currentScore;
		getline(previousScoreFile, currentScore);
		return currentScore; //returning string with data.
		previousScoreFile.close();
	}
	else if (previousScoreFile.fail()) //if it failed.
	{
		return to_string(playerPreviousScore); //converting int to string and returning it.
	}
}

void replayLogger(const int key)
{
	ofstream replayFile("replay_file", std::ios_base::app | std::ios_base::out); //opening file in APPEND mode so we do not delete previous lines...
	if (replayFile.is_open())
	{
		replayFile << to_string(key) + "\n";
	}
}

void replaySystem(vector<int> keys)
{
	fstream replayFile;
	replayFile.open("replay_file"); //opening file
	while (replayFile.is_open())
	{
		string currentK;
		getline(replayFile, currentK); //getting line from replayFile into currentKey var.
		int currentKey = stoi(currentK); //conversion from string to int
		keys.push_back(currentKey); //returning output.
	}
}

void resetFiles()
{
	ofstream replayFile("replay_file");
}
//---------------------------------------------------------------------------
//----- End of Text File Functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//----- Highscores....
//---------------------------------------------------------------------------
void showHighscores()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message); //to show messages

	Clrscr();
	vector<Data> playerData;
	ifstream bestScores("bestScores.txt"); //opening file in APPEND mode so we do not delete previous lines...

	string currentPlayer;
	while (getline(bestScores, currentPlayer))
	{
		string firstScore = currentPlayer.substr(0, currentPlayer.find(" "));
		int x = stoi(firstScore);
		playerData.push_back(Data(x, currentPlayer)); // throw that line into vector
	}
	while (playerData.size() < 6) //if less than 6 data is inside add some more. 
	{
		playerData.emplace_back(-1, "-1    anonymous");
	}

	sort(playerData.begin(), playerData.end()); //sort it, after everything is done.

	for (int i = 0; i < 6; ++i) //showing only first 6. 
	{
		SelectBackColour(clBlack);
		cout << playerData[i].getStr() << endl;
	}
	cout << "\n\n\n\t\t\tPress T again to go back to the game ...";
}
//---------------------------------------------------------------------------
//----- End of Highscores....
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
	void replayLogger(const int key);
	bool isArrowKey(const int k);
	assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		replayLogger(key); //register key press
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		replayLogger(key);//register key press
		break;
	case UP: 	//when UP arrow pressed...
		dx = 0;	//decrease the Y coordinate
		dy = -1;
		replayLogger(key);//register key press
		break;
	case DOWN: 	//when DOWN arrow pressed...
		dx = 0;	//increase the Y coordinate
		dy = +1;
		replayLogger(key);//register key press
		break;
	}
}

int _getKeyPress() //gets keys non-stop rather than on user-basis.
{
	int getKeyPress(); //declare function which will be used for returning purposes.
	if (!_kbhit()) return -1; // will loop until a key is pressed
	else return getKeyPress(); //return it to the getKeyPress function...
}
int getKeyPress()
{ //get key or command (in uppercase) selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	keyPressed = (int)toupper(keyPressed);
	return keyPressed;		//return it in uppercase 
}

bool Eat(const int key)
{
	return key == EAT;
}
bool Exterminate(const int key)
{
	return key == EXTERMINATE;
}
bool Freeze(const int key)
{
	return key == FREEZE;
}
bool Replay(const int key)
{
	return key == REPLAY;
}
bool Highscores(const int key)
{
	return key == HIGHSCORE;
}
bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return key == QUIT;
}
//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------
string startScreen()
{
	string playerName;
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	int levelSelection();

	showMessage(clDarkGrey, clYellow, 60, 1, "DATE: " + GetDate());
	showMessage(clDarkGrey, clYellow, 60, 2, "TIME: " + GetTime());
	showMessage(clDarkGrey, clYellow, 60, 5, "TO MOVE USE KEYBOARD ARROWS ");
	showMessage(clDarkGrey, clYellow, 60, 6, "TO FREEZE ZOMBIES PRESS'F'  ");
	showMessage(clDarkGrey, clYellow, 60, 7, "TO KILL ZOMBIES PRESS'X'    ");
	showMessage(clDarkGrey, clYellow, 60, 8, "TO EAT PILLS PRESS'E'	");
	showMessage(clDarkGrey, clYellow, 60, 9, "TO QUIT ENTER 'Q'           ");
	showMessage(clDarkGrey, clYellow, 10, 4, "----------------------");
	showMessage(clDarkGrey, clYellow, 10, 5, "|| SPOT AND ZOMBIES ||");
	showMessage(clDarkGrey, clYellow, 10, 6, "----------------------");
	showMessage(clDarkGrey, clYellow, 10, 13, "GROUP X -             2017-18");
	showMessage(clDarkGrey, clYellow, 10, 14, "Adrian Chudzikiewicz b7030155");



	showMessage(clDarkGrey, clYellow, 10, 20, "Enter your name: "); showMessage(clDarkGrey, clRed, 26, 20, "");	cin >> playerName;

	Clrscr();

	return playerName;
}

int levelSelection()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	/* LEVEL SELECTION BEGINS HERE*/
	int level;
	showMessage(clBlack, clRed, 32, 6, "PLEASE SELECT YOUR LEVEL");
	showMessage(clBlack, clGreen, 32, 7, "For Level 1 : write 1");
	showMessage(clBlack, clCyan, 32, 8, "For Level 2 : write 2");
	showMessage(clBlack, clYellow, 32, 9, "For Level 3 : write 3");
	showMessage(clBlack, clRed, 32, 10, "For Level 4 : write 4");
	showMessage(clBlack, clDarkCyan, 32, 12, "Chose your level: "); cin >> level;
	showMessage(clDarkGrey, clDarkCyan, 32, 12, ""); //clrscreen uses this colour.


	while (level < 0 || level > 4)
	{
		//FIX
		cin.clear();  //clear the buffer
		cin.ignore(); //ignore previous input
		showMessage(clBlack, clDarkCyan, 32, 12, "Invalid level, please try again: "); cin >> level; //ask again
	}

	Clrscr();

	return level;

	Clrscr();
	/* ENDS HERE*/
}

void mainMenu()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	/* LEVEL SELECTION BEGINS HERE*/
	string option;
	showMessage(clBlack, clRed, 32, 6, "PLEASE SELECT ONE OF YOUR DESIRED OPTIONS");
	showMessage(clBlack, clGreen, 32, 7, "'P' key (for ‘Play Game')	 ");
	showMessage(clBlack, clCyan, 32, 8, "'S' key (for ‘See Score')	 ");
	showMessage(clBlack, clYellow, 32, 9, "'R' key (for ‘See Rules')	");
	showMessage(clBlack, clRed, 32, 10, "'Q' key (for ‘Quit Game')	");
	showMessage(clBlack, clDarkCyan, 32, 12, "Your option is: "); cin >> option;
	showMessage(clDarkGrey, clDarkCyan, 32, 12, ""); //clrscreen uses this colour.

	if (option == "R" || option == "r")
	{
		showMessage(clBlack, clRed, 32, 6, "RULES, THE GREAT RULES, EVERYONE SHOULD OBEY");
		showMessage(clBlack, clGreen, 32, 7, "When Spot falls into a hole, he loses one life ");
		showMessage(clBlack, clCyan, 32, 8, "When Spot hits zombie, he loses one life and zombie resets");
		showMessage(clBlack, clYellow, 32, 9, "Eat power pills to gain impressive powers they will help you win the game");
		showMessage(clBlack, clRed, 32, 10, "Zombies cannot eat power pills");
		showMessage(clBlack, clDarkGreen, 32, 11, "A zombie dies and disappears forever when it falls into a hole.");
		showMessage(clBlack, clDarkCyan, 32, 12, "Read rules quick! You will be in-game within 15 seconds... "); Sleep(15000); //give user 15 seconds to read the rules..
		showMessage(clDarkGrey, clDarkCyan, 32, 12, ""); //clrscreen uses this colour.
	}

	Clrscr();

	Clrscr();
	/* ENDS HERE*/
}
string tostring(int x)
{	//convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
string tostring(char x)
{	//convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message)
{	//display a string using specified colour at a given position 
	Gotoxy(x, y);
	SelectBackColour(backColour);
	SelectTextColour(textColour);
	cout << message;
}
void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string username, string previousBestScore, int& lives, int& pillCount, vector<Item>& zombies, int& moveProtected, bool highScoresView)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	void showHighscores();

	void paintGrid(const char g[][SIZEX], int& moveProtected);
	if (highScoresView == true)
	{
		showHighscores();
	}
	else
	{
		showMessage(clDarkGrey, clYellow, 0, 0, "");
		Clrscr();
		//display game title
		showMessage(clDarkGrey, clYellow, 0, 0, "___SPOT AND ZOMBIES GAME___");
		//Date and Time is displayed when the application starts it gets current time when the application have started.
		showMessage(clDarkGrey, clYellow, 55, 0, "Adrian Chudzikiewicz -  April 18");
		showMessage(clDarkGrey, clYellow, 55, 1, "DATE: " + GetDate()); //Getting exact date.
		showMessage(clDarkGrey, clYellow, 55, 2, "TIME: " + GetTime()); //Getting exact time.

																		//display menu options available
		showMessage(clDarkGrey, clYellow, 55, 4, "TO MOVE USE KEYBOARD ARROWS ");
		showMessage(clDarkGrey, clYellow, 55, 5, "TO FREEZE ZOMBIES PRESS 'F' ");
		showMessage(clDarkGrey, clYellow, 55, 6, "TO KILL ZOMBIES PRESS 'X'   ");
		showMessage(clDarkGrey, clYellow, 55, 7, "TO EAT PILLS PRESS'E'		  ");
		showMessage(clDarkGrey, clYellow, 55, 8, "TO VIEW HIGHSCORES PRESS 'T'");
		showMessage(clDarkGrey, clYellow, 55, 9, "TO QUIT ENTER 'Q'           ");

		// display statistics
		showMessage(clDarkGrey, clYellow, 55, 12, "REMAINING LIVES: " + to_string(lives) + "	");
		showMessage(clDarkGrey, clYellow, 55, 13, "ZOMBIES LEFT: " + to_string(zombies.size()));
		showMessage(clDarkGrey, clYellow, 55, 14, "PILLS LEFT: " + to_string(pillCount));
		showMessage(clDarkGrey, clYellow, 55, 15, "PROTECTED MOVES LEFT: " + to_string(moveProtected) + "	");

		string croppedUsername = username.substr(0, 20); // selecting username string, DISPLAYING ONLY first 20 characters. 
		showMessage(clDarkGrey, clYellow, 55, 25, "PLAYER'S NAME: " + croppedUsername);
		showMessage(clDarkGrey, clYellow, 55, 26, "PLAYER'S PREVIOUS BEST SCORE: " + previousBestScore);


		showMessage(clDarkGrey, clYellow, 0, 18, mess);	//display current message
		showMessage(clDarkGrey, clYellow, 0, 20, "SPOT :" + spotMess);	//display current message
		showMessage(clDarkGrey, clYellow, 0, 21, "ZOMBIE(S) :" + zombieMess);	//display current message

																				//display grid contents
		paintGrid(g, moveProtected);
	}
}
void paintGrid(const char g[][SIZEX], int& moveProtected)
{ //display grid content on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);

	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row) // making sure action is happening for each row of Y.
	{
		for (int col(0); col < SIZEX; ++col) // each row and column.
		{
			switch (g[row][col])
			{
			case HOLE: // if HOLE, then change its colour.
				SelectTextColour(clRed);
				cout << g[row][col]; // redraw new grid.
				break;
			case PILL:
				SelectTextColour(clYellow);
				cout << g[row][col];
				break;
			case WALL:
				SelectTextColour(clGrey);
				cout << g[row][col];
				break;
			case TUNNEL:
				cout << g[row][col];
				break;
			case SPOT:
				if (moveProtected > 0)
					SelectTextColour(clPurple); //change colour to PURPLE when user got protected moves
				else
					SelectTextColour(clWhite);
				cout << g[row][col];
				break;
			case ZOMBIE:
				SelectTextColour(clGreen);
				cout << g[row][col];
				break;
			}
		}
		cout << endl;
	}
}

void endProgram(string quitReason, int& lives, string previousBestScore, string username, bool playerCheated, int& levelScore)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void saveScore(string username, int& lives, string previousBestScore, int& levelScore);
	int measureTime(); //to get final time result.

	const int finalTime = measureTime()/1000; //divide by 1000 to get seconds, time is in miliseconds. 

	if (quitReason == "OutOfLives")
	{
		showMessage(clWhite, clRed, 55, 16, "OUT OF LIVES, YOU LOST... ");
		showMessage(clWhite, clRed, 2, 24, "It took you " + to_string(finalTime) + "seconds to lose.");
		showMessage(clWhite, clRed, 2, 25, "You achieved score of " + to_string(levelScore) + ".");
		if (!playerCheated) // if player did not cheat
		{
			saveScore(username, lives, previousBestScore, levelScore); //save score
		}
		showMessage(clWhite, clRed, 55, 28, "Try again later..."); Sleep(5000); exit(0); //display message, wait 5 seconds, terminate.
	}
	else if (quitReason == "GameFinished")
	{
		showMessage(clWhite, clRed, 55, 16, "VICTORY! NO MORE ZOMBIES, ALL LEVELS PASSED");
		showMessage(clWhite, clRed, 55, 17, "YOU WIN WITH " + to_string(lives) + "LIVES LEFT!");
		showMessage(clWhite, clRed, 2, 24, "It took you " + to_string(finalTime) + "seconds to win.");
		showMessage(clWhite, clRed, 2, 25, "You achieved score of " + to_string(levelScore) + ".");
		if (!playerCheated) // if player cheated is not true
		{
			saveScore(username, lives, previousBestScore, levelScore); // save it
		}
		showMessage(clWhite, clRed, 55, 28, "Congratulations, perhaps one more?"); Sleep(3000); exit(0); //display message, wait 3 seconds, terminate.
	}
	else if (quitReason == "")
	{
		if (!playerCheated)
		{
			saveScore(username, lives, previousBestScore, levelScore);
		}
		showMessage(clWhite, clRed, 55, 28, ""); system("pause");
	}
}
