#include "Game.hpp"
#include <iostream>
#include <stdio.h>

int main(){
	Game *game = new Game();
	while(game->running()){
		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();

	return 0;

}