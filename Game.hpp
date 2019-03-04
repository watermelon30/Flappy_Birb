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
	SDL_Window *window;
	SDL_Renderer* renderer;

};