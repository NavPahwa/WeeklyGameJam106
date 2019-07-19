#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
SDL_RendererFlip flip;
const Uint8* keyState = SDL_GetKeyboardState(NULL);

int gameState = 0;
// 0 Menu
// 1 Game

/* Player Attribs */
int x = 0;

/* Prototype Methods */
void handleEvents();
void updateGame();
void renderOnScreen();
int calculateAnimation(int num1);

/* Classes */
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
		Sprite::loadFile(id, player);
		return true;
	}
	void animate(string id, int width, int height, int currentRow, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		Sprite::animate(width, height, id, currentRow, currentFrame, x, y, flip);
	}
	void update(int numOfFrames)
	{
		currentFrame = Time::calculateAnimTime(numOfFrames);
	}
private:
	int currentFrame;
	string player = "sprites/player.png";
};
#pragma endregion

#pragma region GameStateManagerClass
class GameStateManager {
public:
};
#pragma endregion


/* Characters */
Player player;

/* Main Method */
int main(int argc, char* argv[])
{
	if (gameState == 1)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("Untitled Game", SDL_WINDOWPOS_CENTERED
			, SDL_WINDOWPOS_CENTERED, 720, 480, SDL_WINDOW_SHOWN);
		if (window == NULL)
			cout << "Unable to create Window!" << endl;
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer == NULL)
			cout << "Unable to create Renderer!" << endl;

		player.load("player");
		while (true)
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
		
	}
	return 0;
}

void handleEvents()
{
	switch (event.type)
	{
	case SDL_QUIT:
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_w:
			x += 2;
			flip = SDL_FLIP_NONE;
			break;
		case SDLK_s:
			x -= 2;
			flip = SDL_FLIP_HORIZONTAL;
			break;
		}
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
	player.update(6);
}

void renderOnScreen()
{
	SDL_RenderClear(renderer);
	player.animate("player", 128, 82, 1, x, 0, flip);
	SDL_RenderPresent(renderer);
}