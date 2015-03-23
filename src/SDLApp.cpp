#include <SDLApp.h>
#include <Game.h>
#include <SDL_image.h>
using namespace std;

SDLApp::SDLApp()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw runtime_error(SDL_GetError());
	int initFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	int retFlags = IMG_Init(initFlags);
	if (retFlags != initFlags)
	{
		throw runtime_error(IMG_GetError());
	}

	window_ = NULL;
	renderer_ = NULL;
	cover_ = NULL;
	pages_ = NULL;
}
SDLApp::~SDLApp()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	IMG_Quit();
	SDL_Quit();
}

void SDLApp::Init(const string &title, int width, int height,
	int flags)
{
	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (window_ == NULL)
		throw runtime_error(SDL_GetError());

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);

	cover_ = IMG_LoadTexture(renderer_, "./res/cover.png");
	pages_ = IMG_LoadTexture(renderer_, "./res/pages.png");
}
void SDLApp::Render()
{
	SDL_RenderClear(renderer_);
	SDL_RenderCopy(renderer_, cover_, NULL, NULL);
	SDL_RenderCopy(renderer_, pages_, NULL, NULL);
	SDL_RenderPresent(renderer_);
}

void SDLApp::HandleInput()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
		case SDL_QUIT:
		case SDL_KEYDOWN:
			Command *pCmd = CommandUtils::Parse(ev);
			pCmd->Execute(*Game::GetInstance());
			delete pCmd;
			break;
		}
	}
	

}