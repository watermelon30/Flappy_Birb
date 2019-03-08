#include "Game.hpp"
#include <iostream>
#include <stdio.h>



// #define WINDOW_WIDTH (1200)
// #define WINDOW_HEIGHT (600)


int main(){
	Game *game = new Game();

	game->init("Flappy birb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	while(game->running()){
		game->handleEvents();
		game->update();
		game->render();
	}
	game->clean();
	return 0;

}