#include <stdio.h>
#include "SDL2/SDL.h"


struct Bar{
	SDL_Texture* pic;
	bool upper;
	float x_vel= -10;
	float width, height;
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
	bool isRunning;
	int windowH, windowW;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Surface* barSurface;
	SDL_Surface* birbs[4];

	SDL_Texture* background;
	SDL_Texture* birbPics[4];
	SDL_Texture* currentBirb;
    SDL_Rect dest; // struct to hold the position and size of the sprite

	bool fly = false, dead = false;
	float x_vel =0, y_vel=0, x_pos=50, y_pos= 50;
	Bar bars[6];

	int counter=0, tempCounter=0;

};

