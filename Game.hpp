#include <stdio.h>
#include "SDL2/SDL.h"


struct Birb{
	int midX = 432, midY = 285; //Approximate middle of the birby. 
	int radius = 277; //Radius of the birby picture
	SDL_Texture* birbPics[4];
	SDL_Rect coord; // struct to hold the position and size of the sprite
	float y_vel;
};


struct Bar{
	SDL_Texture* pic;
	SDL_Rect coord;
	float x_pos;
};

struct BarInfo{
	int lowerBarY; //Lowest y position of a lower bar being able to present on screen fully.
	int barDiff; //Difference of lower and upper bar.
	float bar_vel = -6;	//Speed from right window toward left.
	float barGap;	//Gap between each bar.
	const float barGapV = 200;	//Gap between two vertical bars.
	int barHeadH = 35;	//Height of the bar head. (Note: width is the same as image width).
	int bodyHeadDiff = 13; //Difference of bar body and head (one side).
};


class Game{
public:
	Game();
	~Game();
	void init(const char* title, int x, int y, int width, int height);
	void handleEvents();
	void render();
	void clean();
	void update();
	bool running();

private:
	bool isRunning;	//False when game is finished/closed.
	int windowH, windowW;	//Size of the game window.
	const int randomRange = 300;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;

	SDL_Texture* background;

	bool fly = false, dead = false;

	int closestBar = 0;	//The index of the bar that is closest to the birby.
	
	Birb birby;	//Birb object;
	Bar upperbars[6];	//Bars at the upper window.
	Bar lowerbars[6];	//Bars at the lower window.
	BarInfo barInfo;

	int counter=0, tempCounter=0;

	float distance(int x1, int y1, int x2, int y2);
	bool closestFromCirToRec(int x, int y, int rX, int rY, int rW, int rH, int &closestX, int &closestY);
	bool collision(Bar upperbar, Bar lowerbar, BarInfo barInfo, Birb birb);	//Function to check for collision

};

