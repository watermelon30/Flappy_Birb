/**
 * Header file that declares the class and structs this game uses.
 */
#include <stdio.h>
#include <string>
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>


//Birb object
struct Birb{
	int angle = 0;	//For rotating the birb when dying.
	bool fly = false, dead = false;	//Indicator
	int midX = 432, midY = 285; //Approximate coordinate of the middle of the birb picture.
	int radius = 277; //Approximate radius of the birb picture.
	SDL_Texture* birbPics[4];	
	SDL_Rect coord; // struct to hold the position and size of the birb
	int y_vel; //Velocity in y direction (Flying/droping)
};


//Pipe object
struct Pipe{
	SDL_Texture* pic;
	SDL_Rect coord;
};

//For holding the information of pipe. Just need one for the game.
struct PipeInfo{
	int pipeGap;	//Gap between each pipe in parallel.
	int pipeGapV = 200; //Gap between upper and lower pipes.
	int headDist; //Distance from the head of lower pipe to the upper pipe. (pipe height + pipeGapV).
	int pipe_vel = -5;	//Speed of pipes moving from right window toward left.
	int pipeHeadH = 35;	//Height of the pipe head. (Note: width is the same as image width).
	int bodyHeadDiff = 13; //Difference of pipe body and head (one side).

	int highestLowPipePos = 250; //Highest posible lower pipe position in y direction. (50 + pipeGapV)
	int lowestLowPipePos = 550; //Lowest posible lower pipe position in y direction. (50 pixel above boundary)
};


//The text box appears in the top middle of the screen.
struct TextBox{
	int score = 0;
	int highestScore = 0;
	SDL_Texture* display;
	TTF_Font* font;
	SDL_Color color = {100,0,0}; //(r,g,b)
	SDL_Rect coord;
};


//Game class
class Game{
public:
	Game();
	~Game();
	void init(const char* title, int x, int y);
	void handleEvents();
	void render();
	void clean();
	void update();
	bool running();
private:
	const char* scoreFile = "resources/highScore.txt"; //Location of the file the stores the highest score.
	const int windowH = 600, windowW = 1200;//Size of the game window.
	//Initial position of the birby
    const int initialX = 150;
    const int initialY = 50;

	bool isRunning = false;	//False when game is finished/closed.
	bool gameStart = false;
	bool canRestart = false; //To delay user from restarting immediately.
	int infoHeight = -300; //For animating the scoring info block when birby is dead.

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Surface* surface; //For loeading images to sdl surface
	SDL_Texture* background;

	Birb birby;	//Birb object;
	Pipe upperpipes[6];	//Pipes at the upper window.
	Pipe lowerpipes[6];	//Pipes at the lower window.
	PipeInfo pipeInfo; 
	TextBox textBox;

	int closestPipe = 0;	//The index of the pipe that is closest to the birby.
	
	int counter=0, tempCounter=0; //Used to move the wings of birby

	float distance(int x1, int y1, int x2, int y2);
	bool circleRectCollision(int x, int y, int r, int rX, int rY, int rW, int rH, bool &topOfRect);
	bool collision(Pipe upperpipe, Pipe lowerpipe, PipeInfo pipeInfo, Birb birb);
	void randomForLowerPipeY(int &y);
	void changeText(std::string text);
	void displayInfo(bool display);
	void restart();
	void dropping();
};

