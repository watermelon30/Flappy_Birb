#include "Game.hpp"
#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include "SDL2_gfxPrimitives.h"

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


    surface = IMG_Load("resources/background.png");
    if (!surface)
    {
        cout << "error loading background picture: " << SDL_GetError() << endl;
        clean();
        return;
    }

    // load the image data into the graphics hardware's memory
    background = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!background)
    {
        cout << "error creating background: " << SDL_GetError() << endl;
        clean();
        return;
    }

    for(int i=0;i<4;i++){
        string path = "resources/birb" + to_string(i) + ".png";
        surface = IMG_Load(path.c_str());
        if (!surface)
        {
            cout << "error loading birb pictures: " << SDL_GetError() << endl;
            clean();
            return;
        }

        birb[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!birb[i])
        {
            cout << "error creating birb: " << SDL_GetError() << endl;
            clean();
            return;
        }
        // struct to hold the position and size of the sprite
        // get the dimensions of texture
        SDL_QueryTexture(birb[i], NULL, NULL, &dest[i].w, &dest[i].h);
        dest[i].w /= 6;
        dest[i].h /= 6;
    }

    //SDL_SetRenderDrawColor(renderer, 255,255,255,100);
    // circleColor(renderer, 0, 0, 50, 0x008800);
    isRunning = true;
}
 

void Game::handleEvents(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:
			isRunning = false;
			break;
        case SDL_SCANCODE_SPACE:
        case SDL_BUTTON_LEFT:
            fly = true;
        default:
            break;
	}
	return;
}
void Game::render(){
	    // clear the window
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, background, NULL, NULL);

        if(fly){
            y_vel = 10;
        }
        SDL_RenderCopy(renderer, birb[counter], NULL, &dest[counter]);


        // draw the image to the window
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        SDL_Delay(1000/15);

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
    if(counter < 3){
    	counter++;
    } else counter=0;
}
