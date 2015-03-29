#include <GameScene.h>
#include <SDL_image.h>
#include <stdexcept>
#include <Command.h>
#include <Game.h>
using namespace std;

void GameScene::Init(SDL_Renderer * renderer)
{
	cover_ = IMG_LoadTexture(renderer, "./res/cover.png");
	if (cover_ == NULL) throw runtime_error(SDL_GetError());
	pages_ = IMG_LoadTexture(renderer, "./res/pages.png");
	if (pages_ == NULL) throw runtime_error(SDL_GetError());
	playerTexture_ = IMG_LoadTexture(renderer, "./res/Player0.png");
	if (playerTexture_ == NULL) throw runtime_error(SDL_GetError());

	playerSrcRect.x = 1;
	playerSrcRect.y = 1;
	playerSrcRect.w = 14;
	playerSrcRect.h = 16;

	//int windowSizeW, windowSizeH;
	//SDL_GetWindowSize(window_, &windowSizeW, &windowSizeH);
	playerDstRect.x = 1000 / 2 - playerSrcRect.w / 2;
	playerDstRect.y = 700 / 2 - playerSrcRect.h / 2;
	playerDstRect.w = playerSrcRect.w * 2;
	playerDstRect.h = playerSrcRect.h * 2;
}

void GameScene::Update(float seconds)
{

}

void GameScene::Render(SDL_Renderer * renderer)
{
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, cover_, NULL, NULL);
	SDL_RenderCopy(renderer, pages_, NULL, NULL);
	SDL_RenderCopy(renderer, playerTexture_, &playerSrcRect, &playerDstRect);
}

void GameScene::OnEvent(SDL_Event & ev)
{
	Command *pCmd = CommandUtils::Parse(ev);
	pCmd->Execute(*Game::GetInstance());
	delete pCmd;
}