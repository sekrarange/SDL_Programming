#include <GameScene.h>
#include <SDL_image.h>
#include <stdexcept>
#include <Room.h>
using namespace std;

GameScene::GameScene()
{
	gameInstance = NULL;
	cover_ = NULL;
	pages_ = NULL;
	playerTexture_ = NULL;
}

void GameScene::Init(SDL_Renderer * renderer)
{
	gameInstance = Game::GetInstance();
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
	pCmd->Execute(*this);
	delete pCmd;
}

void
GameScene::Execute(QuitCommand & cmd)
{
	gameInstance->SetProperty("running", false);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(MoveCommand & cmd)
{
	if (cmd.m_Dir == kNumDirs)
	{
		cout << "You want to move ... where?\n";
	}
	else
	{
		Room *pNext = gameInstance->GetCurrentRoom()->GetNextRoom(cmd.m_Dir);
		if (pNext)
		{
			// Handle move commands in both rooms
			gameInstance->GetCurrentRoom()->Execute(cmd);
			gameInstance->SetCurrentRoom(pNext);
			gameInstance->GetCurrentRoom()->Execute(cmd);
			// Move player character
			switch (cmd.m_Dir)
			{
			case North:
				playerDstRect.y -= playerDstRect.h;
				break;
			case South:
				playerDstRect.y += playerDstRect.h;
				break;
			case East:
				playerDstRect.x += playerDstRect.w;
				break;
			case West:
				playerDstRect.x -= playerDstRect.w;
				break;
			}
		}
		else
		{
			cout << "You bump your head on the wall. You can't go that way.\n";
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UnknownCommand & cmd)
{
	cout << "Sorry, I did not quite get that.\n";
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(TakeCommand & cmd)
{
	gameInstance->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(DropCommand & cmd)
{
	gameInstance->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(InventoryCommand & cmd)
{
	gameInstance->GetPlayer().Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(LookCommand & cmd)
{
	gameInstance->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(ExamineCommand & cmd)
{
	try
	{
		gameInstance->GetPlayer().Execute(cmd);
	}
	catch (ExamineCommandFailOnPlayerException & ex)
	{
		gameInstance->GetCurrentRoom()->Execute(cmd);
	}

}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UseCommand & cmd)
{
	// try player's inventory use first
	try {
		gameInstance->GetPlayer().Execute(cmd);
	}
	catch (UseCommandFailOnPlayerException & ex)
	{
		// no go, try from room instead.
		gameInstance->GetCurrentRoom()->Execute(cmd);
	}
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(NullCommand & cmd)
{

}