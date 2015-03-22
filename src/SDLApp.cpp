#include <SDLApp.h>
using namespace std;

SDLApp::SDLApp()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw runtime_error(SDL_GetError());
	window_ = NULL;
	renderer_ = NULL;
}
SDLApp::~SDLApp()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void SDLApp::Init(string &title, int width, int height,
	int flags)
{
	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (window_ == NULL)
		throw runtime_error(SDL_GetError());

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);

}
void SDLApp::Render()
{

}
Command * SDLApp::HandleInput()
{
	return NULL;
}