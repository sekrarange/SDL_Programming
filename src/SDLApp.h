#ifndef __SDLApp_h__
#define __SDLApp_h__
#include <SDL.h>
#include <string>
#include <Command.h>

class SDLApp
{
protected:
	SDL_Window * window_;
	SDL_Renderer * renderer_;

public:
	SDLApp();
	virtual ~SDLApp();

	void Init(std::string &title, int width, int height,
		int flags = SDL_WINDOW_SHOWN);
	void Render();
	Command * HandleInput();
};


#endif