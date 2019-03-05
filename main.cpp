#include "Game.hpp"
#include <iostream>
#include <stdio.h>



#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)


int main(){
	Game *game = new Game();

	game->init("Flappy birb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT);
	while(game->running()){
		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();

	return 0;

}