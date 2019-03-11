/**
 * This is the main game file that contains every faunctionality this game has.
 */

#include "Game.hpp"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream> //Read/write files
#include "SDL2/SDL.h" //SDL rendering library
#include <SDL2/SDL_image.h> //Loading image
#include <SDL2/SDL_ttf.h>   //Text

using namespace std;


//Constructor. Read the high score file.
Game::Game(){
    srand(time(NULL));
    ifstream file;
    file.open(scoreFile);
    if(!file.good()){
        cout << "Error reading high score info (located in resources directory)." << endl;
        exit(0);
    }
    string temp;
    getline(file, temp);
    try{
        textBox.highestScore = stoi(temp);
    } catch (invalid_argument & e){
        cout << "Error parsing the highscore file" << endl;
        exit(0);
    }
    file.close();
}

//Destructor
Game::~Game(){}

//Initialise the game state and corresponding variables.
void Game::init(const char* title, int x, int y){

    //Creating game window.
	window = SDL_CreateWindow("Flappy birby", x, y, windowW, windowH, 0);
    if (!window) {
    	cout << "error creating window: " << SDL_GetError() << endl;
        SDL_Quit();
	    return;
    }

    //Creating renderer object for handling rendering.
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (!renderer){
    	cout << "error creating renderer: " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
    }

    //Loading background image into the SDL surface.
    //Source of the background image: https://opengameart.org/content/background-land
    surface = IMG_Load("resources/background.png");
    if (!surface){
        cout << "error loading background picture: " << SDL_GetError() << endl;
        clean();
        return;
    }

    //Loading background image data into the graphics hardware's memory
    background = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);   //Freeing memory
    if (!background){
        cout << "error creating background: " << SDL_GetError() << endl;
        clean();
        return;
    }


    //Loading the birb images.
    // Source of the birb image:
    // https://opengameart.org/content/free-game-asset-grumpy-flappy-bird-sprite-sheets 
    for(int i=0;i<4;i++){
        string path = "resources/birb" + to_string(i) + ".png";
        surface = IMG_Load(path.c_str());
        if (!surface){
            cout << "error loading birb pictures: " << SDL_GetError() << endl;
            clean();
            return;
        }
        birby.birbPics[i] = SDL_CreateTextureFromSurface(renderer, surface);

        if (!birby.birbPics[i]){
            cout << "error creating texture for birb: " << SDL_GetError() << endl;
            clean();
            return;
        }

        //Getting the dimensions of birb.
        SDL_QueryTexture(birby.birbPics[i], NULL, NULL, &birby.coord.w, &birby.coord.h);
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
    birby.midX += initialX; //x position of the middle of the birby. Will never be changed afterward.
    

    //Loading pipe image. Drawn by myself0
    surface = IMG_Load("resources/pipe.png");
    if (!surface){
        cout << "error loading pipe picture: " << SDL_GetError() << endl;
        clean();
        return;
    }

    //Gap between each pipe depends on the size of the birb
    pipeInfo.pipeGap = birby.coord.w * 8;

    //Loading lower pipe textures.
    for(int i=0;i<6;i++){
        lowerpipes[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(lowerpipes[i].pic, NULL, NULL, &lowerpipes[i].coord.w, &lowerpipes[i].coord.h);
        if (!lowerpipes[i].pic){
            cout << "error creating lowerpipes: " << SDL_GetError() << endl;
            clean();
            return;
        }
        //Assigning random position.
        randomForLowerPipeY(lowerpipes[i].coord.y);
        lowerpipes[i].coord.x = windowW + i * pipeInfo.pipeGap;//Alignning each pipe with fixed distance 
    }

    pipeInfo.headDist = lowerpipes[0].coord.h +pipeInfo.pipeGapV;   //Pipe height + gap of two pipes. 

    //Loading upper pipe textures.
    for(int i=0;i<6;i++){
        upperpipes[i].pic = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(upperpipes[i].pic, NULL, NULL, &upperpipes[i].coord.w, &upperpipes[i].coord.h);
        if (!upperpipes[i].pic){
            cout << "error creating upperpipes: " << SDL_GetError() << endl;
            clean();
            return;
        }
        //Upper pipe needs to have a fixed distance with lower pipe in y direction.
        upperpipes[i].coord.y = lowerpipes[i].coord.y - pipeInfo.headDist;

        // Need to be parallel with lower pipe.
        upperpipes[i].coord.x = lowerpipes[i].coord.x;
    }

    //Setting up the text.
    //Initialise sdl ttf library.
    if(TTF_Init()== -1){
        cout << "error initialsing ttf library" << endl;
        return;
    }
    //Loading font
    textBox.font = TTF_OpenFont("resources/monoSans-Bold.otf", 30);
    if(!textBox.font){
        cout << "error loading font: " << SDL_GetError() << endl;
        return;
    }

    surface = TTF_RenderText_Solid(textBox.font, "Press any key to start (Atl+F4 to exit)", textBox.color);

    textBox.display = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(textBox.display, NULL, NULL, &textBox.coord.w, &textBox.coord.h);

    //Place the text box at the top middle of screen.
    textBox.coord.x = windowW / 2 - textBox.coord.w / 2;
    textBox.coord.y = windowH / 6;

    //Freeing the surface from memory.
    SDL_FreeSurface(surface);

    //Start running the game.
    isRunning = true;
    return;
}

 
//Event handler
void Game::handleEvents(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type){
		case SDL_QUIT:
			isRunning = false;

			break;
        case SDL_KEYDOWN:
            //Start game event
            if(!gameStart) {
                changeText("Press any key to fly");
                gameStart = true;
            }

            //Restart event
            if(birby.dead){
                if(canRestart){
                    //restart game when Enter is pressed.
                    if(event.key.keysym.scancode == SDL_SCANCODE_KP_ENTER ||
                       event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                        birby.dead = false;
                        gameStart = true;
                        restart();
                    }
                }
            }
            else{   //birb flies
                birby.fly = true;
            }
            break;
	}
	return;
}

//Rendering
void Game::render(){
    // clear the window
    SDL_RenderClear(renderer);
    
    //Draw background
    SDL_RenderCopy(renderer, background, NULL, NULL);

    //Draw pipes
    for(int i=0;i<6;i++){
        SDL_RenderCopy(renderer, lowerpipes[i].pic, NULL,  &lowerpipes[i].coord);
        //Need to rotate the upper bar with 180 degrees.
        SDL_RenderCopyEx(renderer, upperpipes[i].pic, NULL,  &upperpipes[i].coord, 180, NULL, SDL_FLIP_NONE); 
    }

    //Draw birby.
    SDL_RenderCopyEx(renderer, birby.birbPics[counter], NULL,  &birby.coord, birby.angle, NULL, SDL_FLIP_NONE);

    //Draw text box.
    SDL_RenderCopy(renderer, textBox.display , NULL,  &textBox.coord);

    //Display scoring info when birby hit the pipe.
    displayInfo(birby.dead);


    //Actual rendering
    SDL_RenderPresent(renderer);

    // wait 1/60th of a second (60 fps)
    SDL_Delay(1000/60);

    return;
}

//Updating game state.
void Game::update(){
    //Birb always flying as long as not dead.
    if(!birby.dead){
        if(tempCounter == 4){
            tempCounter=0;
            if(counter==3)counter = 0;
            else counter++;
        }
        else tempCounter++;
    }

    if(gameStart){
        if(birby.fly){
            birby.y_vel = -18; //Flying up
            birby.fly = false;
        }
        else birby.y_vel +=2; //Dropping 
        
        birby.coord.y += birby.y_vel;

        //Let the birby stay on the ground.
        if( birby.coord.y > windowH-birby.coord.h) {
            birby.coord.y = windowH-birby.coord.h;
            birby.y_vel = 0;
        }

        //Face down the birby when dying.
        if(birby.dead) dropping();
        else{
            //Pipes update.
            for(int i=0; i<6;i++){
                lowerpipes[i].coord.x += pipeInfo.pipe_vel;

                //Pipe that disappeared on the screen
                if(lowerpipes[i].coord.x < -lowerpipes[i].coord.w) {
                    //Move to the position of the previous pipe plus pipe gap and pipe velo.
                    lowerpipes[i].coord.x = lowerpipes[(i+5)%6].coord.x + pipeInfo.pipeGap + pipeInfo.pipe_vel;                    

                    //Reassigning the pipes' y position.
                    randomForLowerPipeY(lowerpipes[i].coord.y);
                    upperpipes[i].coord.y = lowerpipes[i].coord.y - pipeInfo.headDist;
                }
                //Move the upper pipe to the top of lower pipe.
                upperpipes[i].coord.x = lowerpipes[i].coord.x;
            }

            //Check if birby has passed the pipe fully.
            if(birby.coord.x < upperpipes[closestPipe].coord.x + upperpipes[closestPipe].coord.w){
                //Checking the collision if the right of the birby is touching the pipe.
                if(birby.coord.x+birby.coord.w > upperpipes[closestPipe].coord.x){
                    if(collision(upperpipes[closestPipe],lowerpipes[closestPipe], pipeInfo, birby)){
                        birby.dead = true;
                        changeText("Oh no you hit the pipe");
                        //Record the highest score.
                        if(textBox.highestScore < textBox.score) {
                            textBox.highestScore = textBox.score;   
                            ofstream file(scoreFile, ofstream::out|ofstream::trunc);
                            if(!file.good()){
                                cout << "Unable to open the high score file" << endl;
                                exit(0);
                            }
                            file << textBox.highestScore;
                            file.close();
                        }
                    }
                }
            }
            //If passed the pipe, add score and change the closest pipe index. 
            else {
                closestPipe = (closestPipe==5)? 0: closestPipe+1;   //The closest pipe move forward.
                textBox.score++;
                changeText(to_string(textBox.score));

                //Speed up the pipe velocity and narrower the gap when passed a certain score.
                if(textBox.score == 10) pipeInfo.pipe_vel--;
                if(textBox.score == 25) {
                    pipeInfo.pipe_vel--; 
                    pipeInfo.headDist-=6;
                }
                if(textBox.score == 40) {
                    pipeInfo.pipe_vel--; 
                    pipeInfo.headDist-=8;
                }
                if(textBox.score == 55) {
                    pipeInfo.pipe_vel--; 
                    pipeInfo.headDist-=10;
                }
                if(textBox.score == 75) {
                    pipeInfo.pipe_vel--; 
                    pipeInfo.headDist-=10;
                }
            }
        }
    }
}

//Distance of two points.
float Game::distance(int x1, int y1, int x2, int y2){
    int dx = x1 - x2;
    int dy = y1 - y2;
    return sqrt(dx*dx + dy*dy);
}


/**
 * This function checks the collision with a circle and a rectangle.
 * @params x, y: Centre of the circle.
 * @param r: Radius of the circle.
 * @params rX, rY: Top left corner coordinate of the rectangle. 
 * @params rW, rH: Width and Height of the rectangle. 
 * @param topOfRect: To check if the circle is on top of the rectangle.
 * return:Whether collision occurs
 */
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
        topOfRect = true;    //Detecting if the point is right above the pipe.
    }
    else if(y > rY + rH) closestY =rY+rH; //Circle is at the bottom of rectangle
    else {  //Circle is touching rectangle in y direction.
        closestY = y;
    }

    //Check the collision.
    if(distance(x, y, closestX, closestY) < float(r)) {return true;}
    
    return false;
}


//Check if the birby has hit the pipe.
//Return: Whether collision occurs with pipes.
bool Game::collision(Pipe upperpipe, Pipe lowerpipe, PipeInfo pipeInfo, Birb birb){

    int midY = birb.coord.y + birb.midY; //Getting the current centre y of the birb.

    bool aboveUpperPipe, aboveLowerPipe; //To check whether the position of the birb is above the pipe heads.

    //Checking collision with lower pipe head.
    if(circleRectCollision(
        birb.midX, midY, birb.radius,
        lowerpipe.coord.x, lowerpipe.coord.y, 
        lowerpipe.coord.w, pipeInfo.pipeHeadH,
        aboveLowerPipe)) return true; 

    //Checking collision with upper pipe head.
    if(circleRectCollision(
        birb.midX, midY, birb.radius,
        upperpipe.coord.x, upperpipe.coord.y + upperpipe.coord.h - pipeInfo.pipeHeadH, //y coordinate of the pipe head
        upperpipe.coord.w, pipeInfo.pipeHeadH,
        aboveUpperPipe)) return true;

    //Checking for collision with pipe body when birby y position is not inside upper and lower pipe heads.
    if(aboveLowerPipe == aboveUpperPipe){
        int pipeBodyX = upperpipe.coord.x + pipeInfo.bodyHeadDiff; //X coordinate of the left side of the pipe body.
        if(abs(pipeBodyX-birb.midX) < birb.radius) return true;
    }
    return false;
}

//Assigning a random position within a range for lower pipe.
void Game::randomForLowerPipeY(int& y){
    //Random y coordinate in a range of highestLowPipePos to lowestLowPipePos
    y = rand() % (pipeInfo.lowestLowPipePos - pipeInfo.highestLowPipePos) + pipeInfo.highestLowPipePos; 
}


//Changing the text of the textbox.
void Game::changeText(string text){
    surface = TTF_RenderText_Solid(textBox.font, text.c_str(), textBox.color);

    textBox.display = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(textBox.display, NULL, NULL, &textBox.coord.w, &textBox.coord.h);
    textBox.coord.x = windowW / 2 - textBox.coord.w / 2;

    SDL_FreeSurface(surface);
}

//Display scoring info only when the birb is dead.
void Game::displayInfo(bool dead){
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

//Reset the cooresponding varibales to the default value.
void Game::restart(){
    changeText("Press any key to fly");

    //Replace te position of the birby.
    
    birby.coord.x = initialX;
    birby.coord.y = initialY;
    birby.angle = 0;

    //Restart the gap between two pipes
    pipeInfo.headDist = lowerpipes[0].coord.h +pipeInfo.pipeGapV;   //Pipe height + gap of two pipes. 
    //Reset the pipe velocity.
    pipeInfo.pipe_vel = -5;

    //Initialise all the pipe coordinate.
    for(int i=0;i<6;i++){
        randomForLowerPipeY(lowerpipes[i].coord.y);
        lowerpipes[i].coord.x = windowW + i * pipeInfo.pipeGap;//windowH - upperpipes[i].coord.h; 
    }
    for(int i=0;i<6;i++){
        upperpipes[i].coord.y = lowerpipes[i].coord.y - pipeInfo.headDist;
        upperpipes[i].coord.x = lowerpipes[i].coord.x;
    }
    
    closestPipe = 0;
    infoHeight = -300;

    //Reset current score.
    textBox.score = 0;
    canRestart = false;
}


//Clean up the necessary objects and quit.
void Game::clean(){
    //TODO: Check high score history.

    SDL_DestroyRenderer(renderer);  //This will free all textures.
    SDL_DestroyWindow(window);
    SDL_Quit();
    cout << "Game closed" << endl;
    return;
}


//To produce the face down animation of the birby.
void Game::dropping(){
    //Birby has already faced down.
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

//Indicating whether the game is running.
bool Game::running(){return isRunning;}
