#include "Game.hpp"
#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

using namespace std;



Game::Game(){}
Game::~Game(){}
void Game::init(const char* title, int x, int y, int width, int height){
    windowH = height;
    windowW = width;    
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

        birbPics[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!birbPics[i])
        {
            cout << "error creating birb: " << SDL_GetError() << endl;
            clean();
            return;
        }
        // struct to hold the position and size of the sprite
        // get the dimensions of texture
        SDL_QueryTexture(birbPics[i], NULL, NULL, &dest.w, &dest.h);
        //cout << dest[i].h << " " << dest[i].w;
    }

    barSurface = IMG_Load("resources/pipe.png");
    if (!barSurface)
    {
        cout << "error loading pipe picture: " << SDL_GetError() << endl;
        clean();
        return;
    }

    // load the image data into the graphics hardware's memory
    bars[0].pic = SDL_CreateTextureFromSurface(renderer, barSurface);
    SDL_FreeSurface(barSurface);


    dest.w /= 6;
    dest.h /= 6;
    dest.y = y_pos;
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
            cout << "x clicked" << endl;
			break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
                case SDL_SCANCODE_KP_SPACE:
                case SDL_SCANCODE_UP:
                // case SDL_BUTTON_LEFT:
                    fly = true;
                    break;
            }
            break;
	}
	return;
}
void Game::render(){
	    // clear the window
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, background, NULL, NULL);


        currentBirb = birbPics[counter];
        SDL_RenderCopy(renderer, currentBirb, NULL, &dest);
        SDL_RenderCopy(renderer, bars[0].pic, NULL, NULL);


        // draw the image to the window
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        SDL_Delay(1000/40);

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

    if(tempCounter == 4){
        tempCounter=0;
        if(counter==3)counter = 0;
        else counter++;
    }
    else tempCounter++;

    if(fly){
        dead = false;;
        y_vel = -25; //Flying up
    } 
    
    
    if(!dead) y_vel +=3; //Dropping 
    
    y_pos += y_vel;

    if(y_pos > windowH-dest.h) {
        y_pos = windowH-dest.h;
        y_vel = 0;
        dead = true;
        cout << "d";
    }

    dest.y = y_pos;

    fly = false;

}
