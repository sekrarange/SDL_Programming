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
	SDL_Texture * cover_;
	SDL_Texture * pages_;

public:
	SDLApp();
	virtual ~SDLApp();

	void Init(const std::string &title, int width, int height,
		int flags = SDL_WINDOW_SHOWN);
	void Render();
	void HandleInput();
};


#endif