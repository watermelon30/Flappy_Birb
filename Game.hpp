#include <stdio.h>
#include "SDL2/SDL.h"


struct Birb{
	SDL_Texture* birbPics[4];
	SDL_Rect coord; // struct to hold the position and size of the sprite
	float x_pos;
	float y_vel;
};


struct Bar{
	SDL_Texture* pic;
	SDL_Rect coord;
	float x_pos;
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
	float x_vel =0, x_pos=50, y_pos= 50;
	int lowerBarY; //Lowest y position of a lower bar being able to present on screen fully.
	int barDiff; //Difference of lower and upper bar.
	
	float bar_vel = -6;	//Speed from right window toward left.
	float barGap;	//Gap between each bar.
	float barGapV = 200;	//Gap between two vertical bars.

	Birb birby;	//Birb object;
	Bar upperbars[6];	//Bars at the upper window.
	Bar lowerbars[6];	//Bars at the lower window.

	int counter=0, tempCounter=0;

	int randomYPosForLowerBar();

};

