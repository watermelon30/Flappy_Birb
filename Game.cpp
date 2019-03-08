#include "Game.hpp"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>       /* time */
#include <math.h>
#include <iostream>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace std;



Game::Game(){
    srand(time(NULL));
}
Game::~Game(){}

void Game::init(const char* title, int x, int y){

	window = SDL_CreateWindow("Flappy birby", x, y, windowW, windowH, 0);
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

    //Loading background image.
    surface = IMG_Load("resources/background.png");
    if (!surface){
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


    //Loading birby image.
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
    birby.coord.w /= 15;
    birby.coord.h /= 15;
    birby.radius /= 15;
    birby.midX /= 15;
    birby.midY /= 15;

    //Initialise the original coordinate
    birby.coord.y = initialY;
    birby.coord.x = initialX;
    birby.midX += initialX; //x position of the middle of the birby. Will never change afterward.
    

    //Loading pipe image.
    surface = IMG_Load("resources/pipe.png");
    if (!surface){
        cout << "error loading pipe picture: " << SDL_GetError() << endl;
        clean();
        return;
    }

    //Gap between each bar depends on the size of the birb
    barInfo.barGap = birby.coord.w * 8;

    //The y position where the lower bar will be in screen fully.
    barInfo.lowerBarY = windowH - surface->h;  

    barInfo.highestBarPos = 100 + barInfo.barGapV; //

    //Loading bar textures. 6 for upper, 6 for lower.
    for(int i=0;i<6;i++){
        lowerbars[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(lowerbars[i].pic, NULL, NULL, &lowerbars[i].coord.w, &lowerbars[i].coord.h);
        if (!lowerbars[i].pic){
            cout << "error creating lowerbars: " << SDL_GetError() << endl;
            clean();
            return;
        }
        //Assigning random position.
        randomForLowerBarY(lowerbars[i].coord.y);
        lowerbars[i].coord.x = windowW + i * barInfo.barGap;//windowH - upperbars[i].coord.h; 
    }

    barInfo.headDist = lowerbars[0].coord.h +barInfo.barGapV;

    for(int i=0;i<6;i++){
        upperbars[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(upperbars[i].pic, NULL, NULL, &upperbars[i].coord.w, &upperbars[i].coord.h);
        if (!upperbars[i].pic){
            cout << "error creating upperbars: " << SDL_GetError() << endl;
            clean();
            return;
        }
        //Upper bar needs to have a fixed distance with lower bar in y direction.
        upperbars[i].coord.y = lowerbars[i].coord.y - barInfo.headDist;

        // Need to be parallel with lower bar.
        upperbars[i].coord.x = lowerbars[i].coord.x;
    }

    SDL_FreeSurface(surface);


    //Initialise sdl ttf library.
    if(TTF_Init()== -1){
        cout << "error initialsing ttf library" << endl;
        return;
    }
    textBox.score = 0;
    textBox.font = TTF_OpenFont("resources/monoSans-Bold.otf", 30);
    if(!textBox.font){
        cout << "error loading font: " << SDL_GetError() << endl;
        return;
    }

    textBox.color = {100,0,0};
    surface = TTF_RenderText_Solid(textBox.font, "Press any key to start (Atl+F4 to exit)", textBox.color);

    textBox.display = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(textBox.display, NULL, NULL, &textBox.coord.w, &textBox.coord.h);

    //Place the text box at the top middle of screen.
    textBox.coord.x = windowW / 2 - textBox.coord.w / 2;
    textBox.coord.y = windowH / 6;

    SDL_FreeSurface(surface);

    //SDL_SetRenderDrawColor(renderer, 255,255,255,100);
    // circleColor(renderer, 0, 0, 50, 0x008800);
    isRunning = true;
    return;
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
            if(!gameStart) {
                changeText("Press any key to fly");
                gameStart = true;
            }

            if(birby.dead){
                if(canRestart){
                    if(event.key.keysym.scancode == SDL_SCANCODE_KP_ENTER ||
                       event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                        birby.dead = false;
                        gameStart = true;
                        restart();
                    }
                }
            }
            else{
                birby.fly = true;
            }
            break;
	}
	return;
}


void Game::render(){
	    // clear the window
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, background, NULL, NULL);

        
        for(int i=0;i<6;i++){
            SDL_RenderCopy(renderer, lowerbars[i].pic, NULL,  &lowerbars[i].coord);
            SDL_RenderCopyEx(renderer, upperbars[i].pic, NULL,  &upperbars[i].coord, 180, NULL, SDL_FLIP_NONE);
        }

        // SDL_RenderCopy(renderer, birby.birbPics[counter], NULL, &birby.coord);
        SDL_RenderCopyEx(renderer, birby.birbPics[counter], NULL,  &birby.coord, birby.angle, NULL, SDL_FLIP_NONE);


        SDL_RenderCopy(renderer, textBox.display , NULL,  &textBox.coord);
        displayInfo(birby.dead);


        // draw the image to the window
        SDL_RenderPresent(renderer);

        // wait 1/60th of a second
        SDL_Delay(1000/60);

        return;
}

void Game::update(){
    if(gameStart){
        if(birby.fly){
            birby.y_vel = -18; //Flying up
            birby.fly = false;
        }
         birby.y_vel +=2; //Dropping 
        
        birby.coord.y += birby.y_vel;

        if( birby.coord.y > windowH-birby.coord.h) {
            birby.coord.y = windowH-birby.coord.h;
            birby.y_vel = 0;
        }

        if(birby.dead){
            dropping();
        }
        else{
            if(tempCounter == 4){
                tempCounter=0;
                if(counter==3)counter = 0;
                else counter++;
            }
            else tempCounter++;

            //Bars x positions update.
            for(int i=0; i<6;i++){
                lowerbars[i].coord.x += barInfo.bar_vel;
                //Bar that disappeared on the screen
                if(lowerbars[i].coord.x < -lowerbars[i].coord.w) {
                    //Move to the position of the last bar plus bar gap and bar velo.  
                    lowerbars[i].coord.x = lowerbars[(i+5)%6].coord.x + barInfo.barGap + barInfo.bar_vel;
                    randomForLowerBarY(lowerbars[i].coord.y);
                    //lowerbars[i].coord.y = barInfo.lowerBarY + rand()%randomRange; //Randomly assign a valid position.            
                    upperbars[i].coord.y = lowerbars[i].coord.y - barInfo.headDist;
                }
                //Move the upper bar to the top of lower bar.
                upperbars[i].coord.x = lowerbars[i].coord.x;
            }


            //Check if birby has passed the bar fully.
            if(birby.midX - birby.radius < upperbars[closestBar].coord.x + upperbars[closestBar].coord.w){
                //Checking for collision if the right of the birby is touching the bar.
                if(birby.midX + birby.radius > upperbars[closestBar].coord.x){
                    if(collision(upperbars[closestBar],lowerbars[closestBar], barInfo, birby)){
                        //gameStart = false;
                        birby.dead = true;
                        changeText("Oh shit you hit the pipe");
                        //Record the highest score.
                        if(textBox.highestScore < textBox.score) textBox.highestScore = textBox.score;
                        // isRunning = false;
                    }
                }
            }
            //If passed the bar, add score and change the closest bar. 
            else {
                closestBar = (closestBar==5)? 0: closestBar+1;   //The closest bar move forward.
                textBox.score++;
                changeText(to_string(textBox.score));
                //Speed up the bar moving velocity and narrow the gap.
                if(textBox.score == 10) barInfo.bar_vel--;
                if(textBox.score == 20) {
                    barInfo.bar_vel--; 
                    barInfo.barGapV-=10;

                }
                if(textBox.score == 35) {barInfo.bar_vel--; barInfo.barGapV-=12;}
                if(textBox.score == 50) {barInfo.bar_vel--; barInfo.barGapV-=15;}
                if(textBox.score == 75) {barInfo.bar_vel--; barInfo.barGapV-=20;}
            }
        }
    }
}

float Game::distance(int x1, int y1, int x2, int y2){
    int dx = x1 - x2;
    int dy = y1 - y2;
    return sqrt(dx*dx + dy*dy);
}

bool Game::circleRectCollision(int x, int y, int r, int rX, int rY, int rW, int rH, bool &topOfRect){
    int closestX, closestY;   //Finding the closest point on the rect 
    topOfRect = false;


    //Finding the closest x position from rect body to circle centre.
    if(x < rX) closestX = rX; //Circle is at the left side of rectangle
    else if(x > rX + rW) closestX = rX + rW; //Circle is at the right side of rectangle
    else { //Circle is touching rectangle in x direction.
        closestX = x;
    }

    //Finding the closest y position from rect body to circle centre.
    if(y < rY) { //Circle is on top of rectangle
        closestY =rY;
        topOfRect = true;    //Detecting if the point is right above the bar.
    }
    else if(y > rY + rH) closestY =rY+rH; //Circle is at the bottom of rectangle
    else {  //Circle is touching rectangle in y direction.
        closestY = y;
    }

    //Check the collision.
    if(distance(x, y, closestX, closestY) < float(r)) {return true;}
    
    return false;
}


bool Game::collision(Bar upperbar, Bar lowerbar, BarInfo barInfo, Birb birb){

    int midY = birb.coord.y + birb.midY; //Getting the current centre y of the birb.



    bool aboveUpperBar, aboveLowerBar; //To check whether the position of the birb is above the bar heads.

    //Checking collision with lower bar head.
    if(circleRectCollision(
        birb.midX, midY, birb.radius,
        lowerbar.coord.x, lowerbar.coord.y, 
        lowerbar.coord.w, barInfo.barHeadH,
        aboveLowerBar)) return true; 


    //Checking collision with upper bar head.
    if(circleRectCollision(
        birb.midX, midY, birb.radius,
        upperbar.coord.x, upperbar.coord.y + upperbar.coord.h - barInfo.barHeadH, //y coordinate of the bar head
        upperbar.coord.w, barInfo.barHeadH,
        aboveUpperBar)) return true;

    //Checking for collision with bar body when birby position is not inside upper and lower bar heads.
    if(aboveLowerBar == aboveUpperBar){
        int barBodyX = upperbar.coord.x + barInfo.bodyHeadDiff; //X coordinate of the left side of the bar body.
        if(abs(barBodyX-birb.midX) <birb.radius) return true;
    }
    return false;
}



void Game::randomForLowerBarY(int& y){
    //Random y coordinate in a range of highestBarPos to lowestBarPos
    y = rand() % (barInfo.lowestBarPos - barInfo.highestBarPos) + barInfo.highestBarPos; 
}


void Game::changeText(string text){
    surface = TTF_RenderText_Solid(textBox.font, text.c_str(), textBox.color);

    textBox.display = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(textBox.display, NULL, NULL, &textBox.coord.w, &textBox.coord.h);
    textBox.coord.x = windowW / 2 - textBox.coord.w / 2;

    SDL_FreeSurface(surface);
}



void Game::displayInfo(bool dead){
    //Only display info when the birb is dead.
    if(dead){
        SDL_Texture* box;
        SDL_Rect position;
        string info = "Score: " + to_string(textBox.score) + 
                      "\nBest:  " + to_string(textBox.highestScore) + 
                      "\nPress Enter to start again" + 
                      "\nPress ALT+F4 to exit";

        //TTF_RenderText_Blended_Wrapped; Used to split and display the text in multiple lines
        surface = TTF_RenderText_Blended_Wrapped(textBox.font, info.c_str(), textBox.color, 500);
        box = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(box, NULL, NULL, &position.w, &position.h);
        position.x = textBox.coord.x;        
        if(infoHeight < windowH/2) infoHeight+=10;
        else canRestart = true;
        position.y = infoHeight;
        SDL_RenderCopy(renderer, box , NULL,  &position);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(box);
    }
}

void Game::restart(){
    changeText("Press any key to fly");

    //Replace te position of the birby.
    
    birby.coord.x = initialX;
    birby.coord.y = initialY;
    birby.angle = 0;

    //Initialise all the bar coordinate.
    for(int i=0;i<6;i++){
        randomForLowerBarY(lowerbars[i].coord.y);
        lowerbars[i].coord.x = windowW + i * barInfo.barGap;//windowH - upperbars[i].coord.h; 
    }
    for(int i=0;i<6;i++){
        upperbars[i].coord.y = lowerbars[i].coord.y - barInfo.headDist;
        upperbars[i].coord.x = lowerbars[i].coord.x;
    }
    
    closestBar = 0;
    infoHeight = -300;

    //Reset current score.
    textBox.score = 0;
    canRestart = false;
    barInfo.barGapV = 200;
    barInfo.bar_vel = -5;
}



void Game::clean(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    cout << "Game closed" << endl;
    return;
}


void Game::dropping(){
    //No need to rotate.
    if(birby.angle >= 90){
        birby.angle = 90;
        return;
    }
    //Birb still going up.
    if(birby.y_vel <= 0) return;

    int height = windowH - birby.coord.y + 1 ; //Distance to the ground. Plus one to prevent the denominator as zero.

    int t = (height / birby.y_vel) + 1; //Time to hit the ground. Plus one to prevent the denominator as zero.
    int remainAngle = 90-birby.angle;
    birby.angle += (int) (remainAngle / t); 
    return;   
}

bool Game::running(){return isRunning;}
