#include <IntroScene.h>
#include <SDL_image.h>
#include <stdexcept>
#include <Game.h>
#include <SDL2_gfxPrimitives.h>

void IntroScene::Init(SDL_Renderer * renderer)
{
	splash_ = IMG_LoadTexture(renderer, "res/splash.bmp");
	if (splash_ == NULL) throw std::runtime_error(IMG_GetError());
	alpha = 0;
	state = splash;
}
void IntroScene::Update(float seconds)
{
	if (state == end)
		Game::GetInstance()->SetCurrentScene("Game");
	if (SDL_GetTicks() >= 2000 && state == splash)
		state = fading;
	if (state == fading)
	{
		alpha += 1;
		if (alpha >= 255)
		{
			state = end;
			alpha = 255;
		}
	}	
}
void IntroScene::Render(SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, splash_, NULL, NULL);
	boxRGBA(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, (Uint8)alpha);
}

void IntroScene::OnEvent(SDL_Event & ev)
{
	switch (ev.type)
	{
	case SDL_QUIT:
		Game::GetInstance()->SetProperty("running", false);
		break;
	default:
		break;
	}
}