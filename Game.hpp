#include <stdio.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>


struct Birb{
	bool fly = false, dead = false;
	int midX = 432, midY = 285; //Approximate middle of the birby. 
	int radius = 277; //Radius of the birby picture
	SDL_Texture* birbPics[4];
	SDL_Rect coord; // struct to hold the position and size of the sprite
	float y_vel;
};


struct Bar{
	SDL_Texture* pic;
	SDL_Rect coord;
};

struct BarInfo{
	int barGap;	//Gap between each bar in parallel.
	const int barGapV = 200; //Gap between upper and lower bars.
	int lowerBarY; //Lowest y position of a lower bar being able to present on screen fully.
	int headDist; //Distance of the head of lower and upper bars.
	float bar_vel = -6;	//Speed of bars moving from right window toward left.
	int barHeadH = 35;	//Height of the bar head. (Note: width is the same as image width).
	int bodyHeadDiff = 13; //Difference of bar body and head (one side).
};


struct Score{
	int value;
	SDL_Texture* display;
	TTF_Font* font;
	SDL_Color color;
	SDL_Rect coord;
};


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
	bool isRunning;	//False when game is finished/closed.
	const int windowH = 600, windowW = 1200;//Size of the game window.


	int highestBarPos = 350; //Highest posible lower bar position in y direction.
	int lowestBarPos = 550; //Lowest posible lower bar position in y direction.

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface; //For loeading images to sdl suuface
	SDL_Texture* background;

	Birb birby;	//Birb object;
	Bar upperbars[6];	//Bars at the upper window.
	Bar lowerbars[6];	//Bars at the lower window.
	BarInfo barInfo;
	Score score;



	int closestBar = 0;	//The index of the bar that is closest to the birby.
	
	int counter=0, tempCounter=0; //Used to move the wings of birby

	float distance(int x1, int y1, int x2, int y2);
	bool circleRectCollision(int x, int y, int r, int rX, int rY, int rW, int rH, bool &topOfRect);
	bool collision(Bar upperbar, Bar lowerbar, BarInfo barInfo, Birb birb);	//Function to check for collision
	void randomForLowerBarY(int &y);
	void changeScore();

};

