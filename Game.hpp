#include <stdio.h>
#include "SDL2/SDL.h"

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
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Texture* background;
	SDL_Texture* birb[4];
	SDL_Texture* currentBirb;
    SDL_Rect dest[4]; // struct to hold the position and size of the sprite

	bool fly = false;
	float x_vel =0, y_vel=0, x_pos=0, y_pos=0;

	int counter=0;

};