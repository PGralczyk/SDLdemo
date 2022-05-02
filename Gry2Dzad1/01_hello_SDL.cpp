/*init(), loadMedia(), close() and loadTexture() use source code provided by Lazy Foo' Productions (2004-2020)*/

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

//Screen dimension constants
const int LEVEL_WIDTH = 900;
const int LEVEL_HEIGHT = 400;

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;

const int OBJECT_VELOCITY = 5;

const int PLAYER_SIZE = 30;
const int PLAYER_AMOUNT = 2;
const int PLAYER_RADIUS = 30;
const int TILE_H = 50;
const int TILE_W = 50;

const int TOTAL_TILES = 3072;
const int TOTAL_TEXTURES = 3;



//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

SDL_Texture* gTexture2 = NULL;
SDL_Texture* gTexture3 = NULL;


class Tile {
public:
	Tile(int x, int y, char type);
	int x;
	int y;
	char getType();
	//SDL_Rect getRec();
	SDL_Rect tRec;
private:
	char type;
};

class Player {
public:
	SDL_Rect cRec;
	bool collision;

	void update_coord() {
		coordx = cRec.x + PLAYER_SIZE / 2;
		coordy = cRec.y + PLAYER_SIZE / 2;
	}

	void update_velocity(float velx, float vely) {
		velocity[0] = velx;
		velocity[1] = vely;
	}

	Player(float velx, float vely) {
		cRec.w = PLAYER_SIZE;
		cRec.h = PLAYER_SIZE;
		cRec.x = LEVEL_WIDTH / 2 + SCREEN_WIDTH / 2;
		cRec.y = LEVEL_HEIGHT / 2 + SCREEN_HEIGHT / 2;
		globalX = cRec.x;
		globalY = cRec.y;
		velocity[0] = velx;
		velocity[1] = vely;
		setCollision(false);
		update_coord();
	}

	void move() {
		cRec.x += velocity[0];
		cRec.y += velocity[1];
		update_coord();
	}

	void move(float velX, float velY) {
		cRec.x += velX;
		cRec.y += velY;
		update_coord();
	}

	void setSquare(int x, int y) {
		cRec.x = x;
		cRec.y = y;
		update_coord();
	}

	void render() {
		if (collision) {
			SDL_RenderCopy(gRenderer, gTexture2, NULL, &cRec);
		}
		else {
			SDL_RenderCopy(gRenderer, gTexture, NULL, &cRec);
		}
	}

	void setCollision(bool flag) {
		collision = flag;
	}

	bool getCollision() {
		return collision;
	}

	float getCoX() {
		return coordx;
	}

	float getCoY() {
		return coordy;
	}

	float getSqX() {
		return cRec.x;
	}

	float getSqY() {
		return cRec.y;
	}

	float getVelX() {
		return velocity[0];
	}

	float getVelY() {
		return velocity[1];
	}

	float getGlX() {
		return globalX;
	}

	float getGlY() {
		return globalY;
	}

	void setGlX(float x) {
		globalX = x;
	}

	void setGlY(float x) {
		globalY = x;
	}

private:
	float globalX;
	float globalY;
	float coordx;
	float coordy;
	float velocity[2];
};

float clamp(float x, float min, float max) {
	if (x < min) {
		return min;
	}
	else if (x > max) {
		return max;
	}
	else {
		return x;
	}
}

bool checkCollision(Player* player, Tile* tile)
{
	int vecX = 0;
	int vecY = 0;
	bool flagger = false;
	//Calculate the sides of player
	int leftA = player->getSqX();
	int rightA = player->getSqX() + PLAYER_SIZE;
	int topA = player->getSqY();
	int bottomA = player->getSqY() + PLAYER_SIZE;

	//Calculate the sides of tile
	int leftB = tile->tRec.x;
	int rightB = tile->tRec.x + TILE_W;
	int topB = tile->tRec.y;
	int bottomB = tile->tRec.y + TILE_H;

	// r1 > l2 ^ r2 > l1 ^ b1 > t2 ^ b2 > t1
	// collision occurs

	if ((rightA > leftB) && (rightB > leftA) && (bottomA > topB) && (bottomB > topA))
	{
		int left = rightA - leftB;
		int right = rightB - leftA;
		int top = bottomA - topB;
		int bottom = bottomB - topA;
		std::cout << "Zachodzi kolizja" << "    ";

		flagger = true;

		if (left < right) {
			vecX = -left;
		}
		else {
			vecX = right;
		}
		if (top < bottom) {
			vecY = -top;
		}
		else {
			vecY = bottom;
		}

		if (abs(vecX) < abs(vecY)) vecY = 0;
		else vecX = 0;

		player->setGlX(player->getGlX() + vecX);
		player->setGlY(player->getGlY() + vecY);
	}
	//std::cout << "!" << flagger<< "!-" << "     ";
	return flagger;
}

int collideCircle(Player* player, Tile* tile) {
	int flag = 0;
	float squareX = clamp((player->getSqX() + PLAYER_SIZE / 2), tile->tRec.x, tile->tRec.x + TILE_W);
	float squareY = clamp((player->getSqY() + PLAYER_SIZE / 2), tile->tRec.y, tile->tRec.y + TILE_H);
	float distX = abs((player->getSqX() + PLAYER_SIZE / 2) - squareX);
	float distY = abs((player->getSqY() + PLAYER_SIZE / 2) - squareY);
	float distance = sqrt(pow(distX, 2) + pow(distY, 2));
	float vectorX;
	float vectorY;
	if (distance < PLAYER_SIZE / 2) {
		//flag = true;
		vectorX = (distX / distance * (PLAYER_SIZE / 2 - distance));
		vectorY = (distY / distance * (PLAYER_SIZE / 2 - distance));
		if (player->getSqX() + PLAYER_SIZE / 2 > squareX) {
			player->setGlX(player->getGlX() + vectorX);
		}
		else if (player->getSqX() + PLAYER_SIZE / 2 < squareX) {
			player->setGlX(player->getGlX() - vectorX);
		}

		if (player->getSqY() + PLAYER_SIZE / 2 > squareY) {
			player->setGlY(player->getGlY() + vectorY);
			flag = 2;
		}
		else if (player->getSqY() + PLAYER_SIZE / 2 < squareY) {
			player->setGlY(player->getGlY() - vectorY);
			flag = 1;
		}
	}
	return flag;
}

bool collideCircle(Player* player, Player* tile) {
	bool flag = false;
	float squareX = clamp((player->getSqX() + PLAYER_SIZE / 2), tile->getSqX(), tile->getSqX() + PLAYER_SIZE);
	float squareY = clamp((player->getSqY() + PLAYER_SIZE / 2), tile->getSqY(), tile->getSqY() + PLAYER_SIZE);
	float distX = abs((player->getSqX() + PLAYER_SIZE / 2) - squareX);
	float distY = abs((player->getSqY() + PLAYER_SIZE / 2) - squareY);
	float distance = sqrt(pow(distX, 2) + pow(distY, 2));
	float vectorX;
	float vectorY;
	if (distance < PLAYER_SIZE / 2) {
		flag = true;
		vectorX = (distX / distance * (PLAYER_SIZE / 2 - distance));
		vectorY = (distY / distance * (PLAYER_SIZE / 2 - distance));
		if (player->getSqX() + PLAYER_SIZE / 2 > squareX) {
			player->setGlX(player->getGlX() + vectorX);
		}
		else if (player->getSqX() + PLAYER_SIZE / 2 < squareX) {
			player->setGlX(player->getGlX() - vectorX);
		}

		if (player->getSqY() + PLAYER_SIZE / 2 > squareY) {
			player->setGlY(player->getGlY() + vectorY);
		}
		else if (player->getSqY() + PLAYER_SIZE / 2 < squareY) {
			player->setGlY(player->getGlY() - vectorY);

		}
	}
	return flag;
}



void resetCollisions(Player* players[]) {
	for (int i = 0; i < PLAYER_AMOUNT; i++) {
		players[i]->setCollision(false);
	}
}

void checkCollisions(Player* players[], bool separation, bool bounce) {
	float pomX;
	float pomY;
	int pomBounceX;
	int pomBounceY;
	float distX;
	float distY;
	float distance;
	float vectorX;
	float vectorY;
	for (int i = 0; i < PLAYER_AMOUNT + 100; i++) {
		for (int j = i + 1; j < PLAYER_AMOUNT; j++) {
			distX = abs(players[i]->getCoX() - players[j]->getCoX());
			distY = abs(players[i]->getCoY() - players[j]->getCoY());
			distance = sqrt(pow(distX, 2) + pow(distY, 2));
			if (distance < PLAYER_RADIUS + 1) {
				players[i]->setCollision(true);
				players[j]->setCollision(true);
				if (separation) {
					if (players[i]->getCoX() != players[j]->getCoX() && players[i]->getCoY() != players[j]->getCoY()) {
						vectorX = (distX / distance * (PLAYER_RADIUS - distance)) + 1;
						vectorY = (distY / distance * (PLAYER_RADIUS - distance)) + 1;
						if (players[i]->getCoX() > players[j]->getCoX() && players[i]->getCoY() > players[j]->getCoY()) {
							players[i]->move(vectorX / 2, vectorY / 2);
							players[j]->move(-vectorX / 2, -vectorY / 2);
						}
						else if (players[i]->getCoX() > players[j]->getCoX() && players[i]->getCoY() < players[j]->getCoY()) {
							players[i]->move(vectorX / 2, -vectorY / 2);
							players[j]->move(-vectorX / 2, vectorY / 2);
						}
						else if (players[i]->getCoX() < players[j]->getCoX() && players[i]->getCoY() > players[j]->getCoY()) {
							players[i]->move(-vectorX / 2, vectorY / 2);
							players[j]->move(vectorX / 2, -vectorY / 2);
						}
						else if (players[i]->getCoX() < players[j]->getCoX() && players[i]->getCoY() < players[j]->getCoY()) {
							players[i]->move(-vectorX / 2, -vectorY / 2);
							players[j]->move(vectorX / 2, vectorY / 2);
						}

						if (bounce) {
							pomBounceX = players[i]->getVelX();
							pomBounceY = players[i]->getVelY();
							players[i]->update_velocity(players[j]->getVelY(), -players[j]->getVelX());
							players[j]->update_velocity(pomBounceY, -pomBounceX);
						}

					}
					else {
						players[i]->move(1, 1);
						players[j]->move(-1, -1);
					}
				}

			}

		}
		resetCollisions(players);
	}
}


char Tile::getType() {
	return type;
}

class Texture {
public:
	Texture(char name, std::string path);
	char getName();
	SDL_Texture* getTexture();
	std::string getPath();
private:
	char textureName;
	std::string path;
	SDL_Texture* classTexture;
};

SDL_Texture* Texture::getTexture() {
	return classTexture;
};

Texture::Texture(char name, std::string pathr) {
	textureName = name;
	path = pathr;
	classTexture = loadTexture(pathr);
}

char Texture::getName() {
	return textureName;
}

std::string Texture::getPath() {
	return path;
}

Tile::Tile(int ex, int ey, char dotype) {
	x = ex;
	y = ey;
	tRec.x = ex;
	tRec.y = ey;

	tRec.w = TILE_W;
	tRec.h = TILE_H;

	type = dotype;
}

void setTilTex(Tile* tileMap[], Texture* textureList[]) {
	std::ifstream map("./mapper.txt");
	char typeCheck = ' ';
	std::string path;
	int iterator = 0;
	int posy = 0;
	int posx = 0;
	map >> typeCheck;
	map >> std::noskipws;
	while (typeCheck != '&' && iterator <= TOTAL_TILES) {
		if (typeCheck == '\n') {
			posx = 0;
			posy += TILE_H;
		}
		else if (typeCheck == ' ')
		{
			posx += TILE_W;
		}
		else {
			tileMap[iterator] = new Tile(posx, posy, typeCheck);
			posx += TILE_W;
			iterator++;
		}
		map >> typeCheck;
	}
	map >> std::skipws;
	if (iterator >= TOTAL_TILES)
	{
		std::cout << "ERROR, TILE OVERFLOW";
		return;
	}
	for (int i = 0; i < TOTAL_TEXTURES; i++) {
		map >> typeCheck;
		std::getline(map, path);
		path.insert(0, "./");
		textureList[i] = new Texture(typeCheck, path);
	}
	map.close();
}


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	gTexture = loadTexture("./square.png");
	if (gTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	gTexture2 = loadTexture("./cirql2.png");
	Uint8 alpha = 80;
	SDL_SetTextureAlphaMod(gTexture2, alpha);
	if (gTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	gTexture3 = loadTexture("./arrow.png");
	//SDL_SetTextureAlphaMod(gTexture3, alpha);
	if (gTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

static int lerp(int now, int then, float percent) {
	return (now + (then - now) * percent);
}


void arrow(SDL_Rect* arrow, SDL_Rect* goal, Texture* texture) {
	float vectorX = arrow->x - goal->x;
	float vectorY = arrow->y - goal->y;


	float mytan = vectorY / vectorX;
	float myatan = atan(mytan) * (180.0 / 3.141592653589793238463);
	std::cout << myatan << std::endl;
	//SDL_RenderCopy(gRenderer, textureList[TOTAL_TEXTURES - 1]->getTexture(), NULL, &tilesMap[iterator]->tRec);
	//SDL_RenderCopyEx(gRenderer, gTexture3, NULL, arrow, myatan* (180.0 / 3.141592653589793238463), NULL, SDL_FLIP_NONE);
	if (vectorX < 0) {
		SDL_RenderCopyEx(gRenderer, texture->getTexture(), NULL, arrow, myatan, NULL, SDL_FLIP_NONE);
	}
	else {
		SDL_RenderCopyEx(gRenderer, texture->getTexture(), NULL, arrow, myatan, NULL, SDL_FLIP_HORIZONTAL);
	}

}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		Tile* tilesMap[TOTAL_TILES] = { NULL };
		Texture* textureList[TOTAL_TEXTURES] = { NULL };
		setTilTex(tilesMap, textureList);
		srand(time(NULL));
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Players
			Player playerONE(0, 0);
			Player playerTWO(0, 0);

			playerONE.setCollision(true);

			Player goal(0, 0);

			Tile goaltile( );

			Texture* goalTexture = new Texture(0, "./goal.png");
			Texture* arrowTexture = new Texture(0, "./arrow.png");
			SDL_Rect arrowz;
			arrowz.w = TILE_W;
			arrowz.h = TILE_H;
			arrowz.x = 20;
			arrowz.y = 170;
			//Camera
			SDL_Rect camera;
			int velocityx = 0;
			int velocityy = 0;
			int accelx = 0;
			int accely = 0;
			camera.x = LEVEL_WIDTH / 2;
			camera.y = LEVEL_HEIGHT / 2;
			camera.w = SCREEN_WIDTH;
			camera.h = SCREEN_HEIGHT;

			//Square
			SDL_Rect rec;
			int recx = camera.x + SCREEN_WIDTH / 2;
			int recy = camera.y + SCREEN_HEIGHT / 2;
			rec.x = recx;
			rec.y = recy;
			rec.w = 40;
			rec.h = 40;

			//Circle
			SDL_Rect cir;
			int cirx = camera.x + SCREEN_WIDTH / 2;
			int ciry = camera.y + SCREEN_HEIGHT / 2;
			cir.x = cirx;
			cir.y = ciry;
			cir.w = 30;
			cir.h = 30;

			//Mouse state
			int mousex = 0;
			int mousey = 0;

			bool spawnFlag = true;
			int iter = 0;
			bool reset = true;
			
			//Verlet data
			float wys = 30;
			float odl = 50;
			float pos = 0;
			float vel = 2 * wys * OBJECT_VELOCITY  / odl;
			float acc = 2* wys * (OBJECT_VELOCITY * OBJECT_VELOCITY) / (odl * odl);
			bool stop = false;

			bool canjump = true;
			int result = 0;
			std::cout << std::endl;
			//testfields
			bool mouseflag = false;
			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.button.button == SDL_BUTTON_LEFT) {
						SDL_GetMouseState(&mousex, &mousey);
						mouseflag = true;
					}

					if (e.type == SDL_MOUSEBUTTONUP)
					{
						mouseflag = false;
					}

				}

				if (reset) {
					while (spawnFlag) {
						iter = 0;
						playerONE.setGlX((rand() % 5) * 50 + 10);
						playerONE.setGlY((rand() % 5) * 50 + 10);
						playerONE.setSquare(playerONE.getGlX(), playerONE.getGlY());
						playerTWO.setGlX((rand() % 5) * 50 + 10);
						playerTWO.setGlY((rand() % 5) * 50 + 10);
						playerTWO.setSquare(playerTWO.getGlX(), playerTWO.getGlY());
						std::cout << playerONE.getSqX() << "   " << playerONE.getSqY() << std::endl;
						std::cout << playerTWO.getSqX() << "   " << playerTWO.getSqY() << std::endl;
						spawnFlag = false;
						while (tilesMap[iter] != NULL) {
							tilesMap[iter]->tRec.x = tilesMap[iter]->x;
							tilesMap[iter]->tRec.y = tilesMap[iter]->y;
							if (tilesMap[iter]->getType() == '1') {
								if (checkCollision(&playerONE, tilesMap[iter])) {
									//std::cout << "   &" << checkCollision(&playerONE, tilesMap[iter]) << "&-    "<<std::endl;
									spawnFlag = true;

								}
								if (checkCollision(&playerTWO, tilesMap[iter])) {
									//std::cout << "   &" << checkCollision(&playerTWO, tilesMap[iter]) << "&-    " << std::endl;
									spawnFlag = true;
								}
							}
							iter++;
						}

					}

					/*spawnFlag = true;
					iter = 0;
					while (spawnFlag) {
						iter = 0;
						goal.setGlX((rand() % 8 + 9) * 50 + 10);
						goal.setGlY((rand() % 18) * 50 + 10);
						goal.setSquare(goal.getGlX(), goal.getGlY());
						spawnFlag = false;
						while (tilesMap[iter] != NULL) {
							if (tilesMap[iter]->getType() == '1') {
								if (checkCollision(&goal, tilesMap[iter])) {
									spawnFlag = true;

								}
							}
							iter++;
						}
					}*/
					spawnFlag = true;
					reset = false;
				}
				const Uint8* keyState = SDL_GetKeyboardState(NULL);

				//Player TWO

				if (keyState[SDL_SCANCODE_D] )
				{
					odl++;
					std::cout << "Distance: "<< odl << std::endl;
				}
				else if (keyState[SDL_SCANCODE_A])
				{
					odl--;
					std::cout << "Distance: "<< odl << std::endl;
				}


				if (keyState[SDL_SCANCODE_S])
				{
					wys--;
					std::cout << "Height: "<< wys << std::endl;
				}
				else if (keyState[SDL_SCANCODE_W] && playerTWO.getSqY() >= 0)
				{
					wys++;
					std::cout << "Height: "<< wys << std::endl;
				}

				//Player ONE

				if (keyState[SDL_SCANCODE_RIGHT] && playerONE.getSqX() <= SCREEN_WIDTH - PLAYER_SIZE)
				{
					if (playerONE.getGlX() < LEVEL_WIDTH - PLAYER_SIZE) {

						playerONE.setGlX(playerONE.getGlX() + OBJECT_VELOCITY);//playerONE.setSquare(playerONE.getSqX() + OBJECT_VELOCITY, playerONE.getSqY());
					}
					if (playerONE.getGlX() > camera.x + SCREEN_WIDTH - PLAYER_SIZE) {
						if (!(keyState[SDL_SCANCODE_A]) && playerTWO.getSqX() <= 0) {
							playerTWO.setGlX(playerTWO.getGlX() + OBJECT_VELOCITY);//cirx += OBJECT_VELOCITY;
						}
					}

				}
				else if (keyState[SDL_SCANCODE_LEFT] && playerONE.getSqX() >= 0 /*+ camera.x*/)
				{
					if (playerONE.getGlX() > 0) {
						playerONE.setGlX(playerONE.getGlX() - OBJECT_VELOCITY);
					}
					if (playerONE.getGlX() < camera.x) {
						if (!(keyState[SDL_SCANCODE_D]) && playerTWO.getSqX() >= SCREEN_WIDTH - PLAYER_SIZE) {
							playerTWO.setGlX(playerTWO.getGlX() - OBJECT_VELOCITY);
						}
					}

				}


				if (keyState[SDL_SCANCODE_DOWN] && playerONE.getSqY() <= SCREEN_HEIGHT - PLAYER_SIZE)
				{
					if (playerONE.getGlY() < LEVEL_HEIGHT - PLAYER_SIZE) {
						playerONE.setGlY(playerONE.getGlY() + OBJECT_VELOCITY);
					}
					if (playerONE.getGlY() > camera.y + SCREEN_HEIGHT - PLAYER_SIZE) {
						if (!(keyState[SDL_SCANCODE_W]) && playerTWO.getSqY() <= 0) {
							playerTWO.setGlY(playerTWO.getGlY() + OBJECT_VELOCITY);
						}
					}

				}
				else if (keyState[SDL_SCANCODE_UP] && playerONE.getSqY() >= 0 && canjump)
				{
					if (playerONE.getGlY() > 0) {
						//playerONE.setGlY(playerONE.getGlY() - vel - 0.5 * acc);
						vel = 2 * wys * OBJECT_VELOCITY / odl;
						acc = 2 * wys * (OBJECT_VELOCITY * OBJECT_VELOCITY) / (odl * odl);
						std::cout <<"Velocity: "<< vel << std::endl;
						stop = false;
						canjump = false;
					}
					if (playerONE.getGlY() < camera.y) {
						if (!(keyState[SDL_SCANCODE_S]) && playerTWO.getSqY() >= SCREEN_HEIGHT - PLAYER_SIZE)
							playerTWO.setGlY(playerTWO.getGlY() - OBJECT_VELOCITY);
					}

				}


				camera.x = (playerONE.getGlX()) - SCREEN_WIDTH / 2 + PLAYER_SIZE / 2;
				if (camera.x < 0) {
					camera.x = 0;
				}
				else if (camera.x > LEVEL_WIDTH - camera.w) {
					camera.x = LEVEL_WIDTH - camera.w;
				}
				camera.y = (playerONE.getGlY()) - SCREEN_HEIGHT / 2 + PLAYER_SIZE / 2;
				if (camera.y < 0) {
					camera.y = 0;
				}
				else if (camera.y > LEVEL_HEIGHT - camera.h) {
					camera.y = LEVEL_HEIGHT - camera.h;
				}
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xAA, 0xAA, 0xAA, 0xAA);
				SDL_RenderClear(gRenderer);
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
				if (stop) {
					vel = 0;
				}
				else {
					playerONE.setGlY(playerONE.getGlY() - vel - 0.5 * acc);
					vel -= acc;
				}
				stop = false;
				int iterator = 0;
				while (tilesMap[iterator] != NULL) {
					for (int i = 0; i < TOTAL_TEXTURES; i++) {
						if (tilesMap[iterator]->getType() == textureList[i]->getName()) {
							tilesMap[iterator]->tRec.x = tilesMap[iterator]->x - camera.x;
							tilesMap[iterator]->tRec.y = tilesMap[iterator]->y - camera.y;
							if (textureList[i]->getName() == '1') {
								playerONE.setSquare(playerONE.getGlX() - camera.x, playerONE.getGlY() - camera.y);
								playerTWO.setSquare(playerTWO.getGlX() - camera.x, playerTWO.getGlY() - camera.y);
								result = collideCircle(&playerONE, tilesMap[iterator]);
								
								if ( result == 1) {
									stop = true;
									canjump = true;
								}
								else if (result == 2) {
									vel = 0;
								}
							}
							SDL_RenderCopy(gRenderer, textureList[i]->getTexture(), NULL, &tilesMap[iterator]->tRec);

							break;
						}
					}
					iterator++;
					
				}
				
				//rec.x = recx - camera.x;
				//rec.y = recy - camera.y;
				playerONE.setSquare(playerONE.getGlX() - camera.x, playerONE.getGlY() - camera.y);
				playerTWO.setSquare(playerTWO.getGlX() - camera.x, playerTWO.getGlY() - camera.y);
				//cir.x = cirx - camera.x;
				//cir.y = ciry - camera.y;
				//SDL_RenderCopy(gRenderer, gTexture, NULL, &rec);
				playerONE.render();
				//playerTWO.render();
				//goal.setSquare(goal.getGlX() - camera.x, goal.getGlY() - camera.y);
				//SDL_RenderCopy(gRenderer, goalTexture->getTexture(), NULL, &goal.cRec);
				//arrow(&arrowz, &goal.cRec, arrowTexture);

				if (collideCircle(&goal, &playerONE) || collideCircle(&goal, &playerTWO)) {
					reset = true;
				}
				// 
				//arrow(&playerTWO.cRec, &playerONE.cRec);
				// 
				//SDL_RenderCopy(gRenderer, gTexture2, NULL, &cir);
				//playerONE.setSquare(playerONE.getSqX() + camera.x, playerONE.getSqY() + camera.y);
				//playerTWO.setSquare(playerTWO.getSqX() + camera.x, playerTWO.getSqY() + camera.y);
				//Update screen
				SDL_RenderPresent(gRenderer);

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}