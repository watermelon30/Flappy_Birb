#include <stdio.h>
#include <string>
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>


struct Birb{
	int angle = 0;
	bool fly = false, dead = false;
	int midX = 432, midY = 285; //Approximate middle of the birby. 
	int radius = 277; //Radius of the birby picture
	SDL_Texture* birbPics[4];
	SDL_Rect coord; // struct to hold the position and size of the sprite
	int y_vel;
};


struct Bar{
	SDL_Texture* pic;
	SDL_Rect coord;
};

struct BarInfo{
	int barGap;	//Gap between each bar in parallel.
	int barGapV = 200; //Gap between upper and lower bars.
	int lowerBarY; //Lowest y position of a lower bar being able to present on screen fully.
	int headDist; //Distance of the head of lower and upper bars.
	int bar_vel = -5;	//Speed of bars moving from right window toward left.
	int barHeadH = 35;	//Height of the bar head. (Note: width is the same as image width).
	int bodyHeadDiff = 13; //Difference of bar body and head (one side).

	int highestBarPos = 350; //Highest posible lower bar position in y direction.
	int lowestBarPos = 550; //Lowest posible lower bar position in y direction.
};


struct TextBox{
	int score = 0;
	int highestScore = 0;
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
	bool gameStart = false;
	bool isRunning;	//False when game is finished/closed.
	bool canRestart = false; //To delay user from restarting immediately.
	int infoHeight = -300; //For animating the scoring info block when birby is dead.
	const int windowH = 600, windowW = 1200;//Size of the game window.


	//Initial position of te birby
    const int initialX = 150;
    const int initialY = 50;



	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface; //For loeading images to sdl suuface
	SDL_Texture* background;

	Birb birby;	//Birb object;
	Bar upperbars[6];	//Bars at the upper window.
	Bar lowerbars[6];	//Bars at the lower window.
	BarInfo barInfo;
	TextBox textBox;



	int closestBar = 0;	//The index of the bar that is closest to the birby.
	
	int counter=0, tempCounter=0; //Used to move the wings of birby

	float distance(int x1, int y1, int x2, int y2);
	bool circleRectCollision(int x, int y, int r, int rX, int rY, int rW, int rH, bool &topOfRect);
	bool collision(Bar upperbar, Bar lowerbar, BarInfo barInfo, Birb birb);	//Function to check for collision
	void randomForLowerBarY(int &y);
	void changeText(std::string text);
	void displayInfo(bool display);
	void restart();
	void dropping();

};

