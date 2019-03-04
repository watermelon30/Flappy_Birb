#include "Game.hpp"
#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"

using namespace std;


Game::Game(){}
Game::~Game(){}
void Game::init(const char* title, int x, int y, int width, int height){
	 window = SDL_CreateWindow("Flappy birby", x, y, width, height, 0);
    if (!window) {
    	cout << "error creating window: " << SDL_GetError() << endl;
        SDL_Quit();
	    return;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (!renderer){
    	cout << "error creating renderer: " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
    }
    SDL_SetRenderDrawColor(renderer, 255,255,200,255);
    isRunning = true;
}
 

void Game::handleEvents(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:
			isRunning = false;
			break;

	}
	return;

}
void Game::render(){
	    // clear the window
        SDL_RenderClear(renderer);

        // draw the image to the window
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        SDL_Delay(1000/60);

        return;
}

void Game::clean(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    cout << "Game closed" << endl;
    return;

}


bool Game::running(){return isRunning;}

void Game::update(){
	
}
