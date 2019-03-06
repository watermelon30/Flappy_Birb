#include "Game.hpp"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>       /* time */
#include <iostream>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

using namespace std;



Game::Game(){
    srand(time(NULL));
}
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
    if (!background){
        cout << "error creating background: " << SDL_GetError() << endl;
        clean();
        return;
    }

    for(int i=0;i<4;i++){
        string path = "resources/birb" + to_string(i) + ".png";
        surface = IMG_Load(path.c_str());
        if (!surface){
            cout << "error loading birb pictures: " << SDL_GetError() << endl;
            clean();
            return;
        }

        birby.birbPics[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!birby.birbPics[i]){
            cout << "error creating birb: " << SDL_GetError() << endl;
            clean();
            return;
        }
        // struct to hold the position and size of the sprite
        // get the dimensions of texture
        SDL_QueryTexture(birby.birbPics[i], NULL, NULL, &birby.coord.w, &birby.coord.h);
        //cout << coord.h << " " << coord.w;
    }

    //Scaling down the birb size;
    birby.coord.w /= 8;
    birby.coord.h /= 8;
    birby.coord.y = y_pos;
    birby.coord.x = 50;



    surface = IMG_Load("resources/pipe.png");
    if (!surface){
        cout << "error loading pipe picture: " << SDL_GetError() << endl;
        clean();
        return;
    }


    barGap = birby.coord.w * 2.5;


    lowerBarY = windowH - surface->h; //The y position where the lower bar will be in screen fully. 

    for(int i=0;i<6;i++){
        lowerbars[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(lowerbars[i].pic, NULL, NULL, &lowerbars[i].coord.w, &lowerbars[i].coord.h);
        if (!lowerbars[i].pic){
            cout << "error creating lowerbars: " << SDL_GetError() << endl;
            clean();
            return;
        }
        lowerbars[i].coord.y =  lowerBarY + rand()%randomRange; 
        lowerbars[i].coord.x = windowW + i * barGap;//windowH - upperbars[i].coord.h; 
    }

    barDiff = surface->h +barGapV;

    for(int i=0;i<6;i++){
        upperbars[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(upperbars[i].pic, NULL, NULL, &upperbars[i].coord.w, &upperbars[i].coord.h);
        if (!upperbars[i].pic){
            cout << "error creating upperbars: " << SDL_GetError() << endl;
            clean();
            return;
        }
        upperbars[i].coord.y = lowerbars[i].coord.y - barDiff;
        upperbars[i].coord.x = lowerbars[i].coord.x;//windowH - upperbars[i].coord.h; 
    }


    SDL_FreeSurface(surface);




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

        SDL_RenderCopy(renderer, birby.birbPics[counter], NULL, &birby.coord);
        
        for(int i=0;i<6;i++){
            SDL_RenderCopy(renderer, lowerbars[i].pic, NULL,  &lowerbars[i].coord);
            SDL_RenderCopyEx(renderer, upperbars[i].pic, NULL,  &upperbars[i].coord, 180, NULL, SDL_FLIP_NONE);
        }


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

    if(tempCounter == 4){
        tempCounter=0;
        if(counter==3)counter = 0;
        else counter++;
    }
    else tempCounter++;

    if(fly){
        birby.y_vel = -25; //Flying up
        fly = false;
    } 
    if(!dead) birby.y_vel +=2; //Dropping 
    
     birby.coord.y += birby.y_vel;

    if( birby.coord.y > windowH-birby.coord.h) {
        birby.coord.y = windowH-birby.coord.h;
        birby.y_vel = 0;
    }

    //Bars x positions update.
    for(int i=0; i<6;i++){
        lowerbars[i].coord.x += bar_vel;
        if(lowerbars[i].coord.x < -lowerbars[i].coord.w) {
            //Move to the back of the last bar with bar gap and  
            lowerbars[i].coord.x = lowerbars[(i+5)%6].coord.x + barGap + bar_vel;   
            lowerbars[i].coord.y = lowerBarY + rand()%randomRange; //Randomly assign a valid position.            
            upperbars[i].coord.y = lowerbars[i].coord.y - barDiff;
        }
        upperbars[i].coord.x = lowerbars[i].coord.x;
    }
}


int Game::randomYPosForLowerBar(){

}
