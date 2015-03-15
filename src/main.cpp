// Copyright anssi.grohn@karelia.fi (c) 2014.
// Licensed under GPLv3.
#include "Game.h"
#include <iostream>
#include <string>
#include <SDL.h>
////////////////////////////////////////////////////////////////////////////////
using namespace std;
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw runtime_error(SDL_GetError());
	
	SDL_Window *pScreen = SDL_CreateWindow("QuickEscape", 100, 100, 640, 400, SDL_WINDOW_SHOWN);
	if (pScreen == NULL)
		throw runtime_error(SDL_GetError());

	SDL_Renderer *renderer = SDL_CreateRenderer(pScreen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Surface *surface = SDL_LoadBMP("./res/splash.bmp");
	if (surface == NULL)
		throw runtime_error(SDL_GetError());

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	// after a delay the splash screen closes before starting the game
	SDL_Delay(2000);
	SDL_DestroyWindow(pScreen);
	
	Game & g = *Game::GetInstance();
	g.Play();

	SDL_Quit();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
