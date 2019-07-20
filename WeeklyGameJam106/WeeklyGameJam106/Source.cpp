/*

TODO:
	(1) Map Class

*/
/* Standard Library Includes */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

/* SDL Includes */
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
SDL_RendererFlip flip;
const Uint8* keyState = SDL_GetKeyboardState(NULL);
bool isRunning = true;

int gameState = 0;
// 0 Menu
// 1 Game

/* Player Attribs */
int x = 0;
int playFrame = 6;

/* Prototype Methods */
void handleEvents();
void updateGame();
void renderOnScreen();
int calculateAnimation(int num1);
void checkPlayer();

/* Classes */
#pragma region MapClass
class Map {
public:
	Map();
	~Map();
private:
	SDL_Rect src;
	SDL_Rect dest;
	
	SDL_Texture* grass;
	SDL_Texture* water;
	SDL_Texture* rocks;

	int map[20][25];
};
#pragma endregion

#pragma region TextClass
class Text {
public:
	Text(string text, int fontSize = 24)
	{
		TTF_Font* Sans = TTF_OpenFont("Rubik-Light.ttf", 20); //this opens a font style and sets a size
		if (Sans == NULL)
			cout << "unable to open file!";
		SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "Navyan", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		SDL_QueryTexture(Message, NULL, NULL, &width, &height);
		SDL_Rect Message_rect; //create a rect
		Message_rect.x = 100;  //controls the rect's x coordinate 
		Message_rect.y = 100; // controls the rect's y coordinte
		Message_rect.w = width; // controls the width of the rect
		Message_rect.h = height; // controls the height of the rect

		//Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

		//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	}
private:
	TTF_Font* font;
	SDL_Rect fontRect;
	SDL_Surface* textSurface;
	SDL_Texture* texture;
	SDL_Color color;
	int width, height;
};
#pragma endregion

#pragma region TimeClass
class Time {
public:
	static int calculateAnimTime(int frames)
	{
		int num = int((SDL_GetTicks() / 100) % frames);
		return num;
	}
};
#pragma endregion

#pragma region SpriteClass
class Sprite {
public:
	bool loadFile(string id, string path)
	{
		SDL_Surface* tmpSurface = IMG_Load(path.c_str());
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
		if (texture == NULL)
			return false;
		textureMap[id] = texture;
		return true;
	}
	void draw(int width, int height, string id, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = destRect.w = width;
		srcRect.h = destRect.h = height;
		destRect.x = x;
		destRect.y = y;
		SDL_RenderCopyEx(renderer, textureMap[id], &srcRect, &destRect, 0, 0, flip);
	}
	void animate(int width, int height, string id, int currentRow, int currentFrame, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		srcRect.x = width * currentFrame;
		srcRect.y = height * (currentRow - 1);
		srcRect.w = destRect.w = width;
		srcRect.h = destRect.h = height;
		destRect.x = x;
		destRect.y = y;
		SDL_RenderCopyEx(renderer, textureMap[id], &srcRect, &destRect, 0, 0, flip);
	}
private:
	SDL_Rect srcRect;
	SDL_Rect destRect;
	int currentFrame;
	map<string, SDL_Texture*> textureMap;
};
#pragma endregion

#pragma region PlayerClass
class Player : public Sprite {
public:
	bool load(string id)
	{
		Sprite::loadFile(id, playerTex);
		return true;
	}
	void animate(string id, int width, int height, int currentRow, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		Sprite::animate(width, height, id, currentRow, currentFrame, x, y, flip);
	}
	void update(int numOfFrames)
	{
		currentFrame = Time::calculateAnimTime(numOfFrames);
		isUpdating = true;
	}

	string playerTex = "sprites/player.png";
	bool isUpdating = true;
private:
	int currentFrame;
};
#pragma endregion

/* Characters */
Player player;

/* Main Method */
int main(int argc, char* argv[])
{
	if (gameState == 0)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		window = SDL_CreateWindow("Untitled Game", SDL_WINDOWPOS_CENTERED
			, SDL_WINDOWPOS_CENTERED, 720, 480, SDL_WINDOW_SHOWN);
		if (window == NULL)
			cout << "Unable to create Window!" << endl;
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer == NULL)
			cout << "Unable to create Renderer!" << endl;

		player.load("player");
		while (isRunning)
		{
			SDL_PollEvent(&event);
			handleEvents();
			renderOnScreen();
			updateGame();
		}
		SDL_Quit();
	}
	else
	{
		while (true)
			handleEvents();
	}
	return 0;
}

void handleEvents()
{
	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_w:
			player.isUpdating = true;
			playFrame = 6;
			checkPlayer();
			x += 2;
			flip = SDL_FLIP_NONE;
			break;
		case SDLK_s:
			player.isUpdating = true;
			playFrame = 6;
			checkPlayer();
			x -= 2;
			flip = SDL_FLIP_HORIZONTAL;
			break;
		default:
			player.isUpdating = false;
			playFrame = 1;
			break;
		}
		break;
	default:
		break;
	}
}

int calculateAnimation(int num1)
{
	int num = int((SDL_GetTicks() / 100) * num1);
	return num;
}

void updateGame()
{
	if (player.isUpdating)
	{
		player.update(playFrame);
	}
	else
	{
		;
	}
}

void renderOnScreen()
{
	checkPlayer();
	SDL_RenderClear(renderer);
	player.animate("player", 128, 82, 1, x, 0, flip);
	Text text("Navyan");
	SDL_RenderPresent(renderer);
}

void checkPlayer()
{
	if (x >= 572)
	{
		x = 572;
		playFrame = 1;
	}
	else 
	{
		playFrame = 6;
	}
	if (x <= 0)
	{
		x = 0;
		playFrame = 1;
	}
	else
	{
		playFrame = 6;
	}
}