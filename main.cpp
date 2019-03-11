/**
 * Main file. The whole source code of the game can be found in my github account:
 * https://github.com/watermelon30/Flappy_Birb
*/
#include "Game.hpp"
#include <iostream>
#include <stdio.h>


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