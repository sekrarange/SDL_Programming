#include <GameScene.h>
#include <SDL_image.h>
#include <stdexcept>
#include <Room.h>
#include <SDL2_gfxPrimitives.h>
#include <tinyxml2.h>

using namespace std;
using namespace tinyxml2;

Page  GameScene::Page_ = Page();

GameScene::GameScene()
{
	gameInstance = NULL;
	cover_ = NULL;
	pages_ = NULL;
	playerTexture_ = NULL;
	font_ = NULL;
}
GameScene::~GameScene()
{
	TTF_CloseFont(font_);
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

	playerDstRect.x = WINDOW_WIDTH / 2 - playerSrcRect.w / 2;
	playerDstRect.y = WINDOW_HEIGHT / 2 - playerSrcRect.h / 2;
	playerDstRect.w = playerSrcRect.w * 2;
	playerDstRect.h = playerSrcRect.h * 2;

	LoadFont("res/font.xml");

	if (font_ == NULL)
		throw runtime_error(TTF_GetError());

	PageRect = { 45, 70, 425, 560 };;
	Page_.Init(renderer, 425, 560);
	Page_._pageFont = font_;
	Page_._justification = Justification::Left;

	state = fadeIn;
	alpha = 255;
}

void GameScene::LoadFont(const std::string & filename)
{
	XMLDocument doc;
	doc.LoadFile(filename.c_str());
	if (doc.ErrorID() != XML_NO_ERROR)
	{
		ostringstream ss;
		ss << "Loadfile: " << doc.GetErrorStr1() << " " << doc.GetErrorStr2();
		throw XmlParsingException(ss.str());
	}

	XMLElement *elem = doc.FirstChildElement("Font");

	if (elem != NULL)
	{
		stringstream ss;
		ss << elem->Attribute("path") << elem->Attribute("name");
		string filepath;
		ss >> filepath;
		int size = elem->IntAttribute("size");
		font_ = TTF_OpenFont(filepath.c_str(), size);
	}
}

void GameScene::Update(float seconds)
{
	if (state == fadeIn)
	{
		alpha -= 1;
		if (alpha <= 0)
		{
			alpha = 0;
			state = play;
		}
	}
	Page_.Compose(font_);
}

void GameScene::Render(SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, cover_, NULL, NULL);
	SDL_RenderCopy(renderer, pages_, NULL, NULL);
	SDL_RenderCopy(renderer, playerTexture_, &playerSrcRect, &playerDstRect);
	Page_.RenderContent(renderer);
	SDL_RenderCopy(renderer, Page_._pageTexture, NULL, &PageRect);
	if (state == fadeIn)
		boxRGBA(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 255, 255, 255, (Uint8)alpha);
}

void GameScene::OnEvent(SDL_Event & ev)
{
	Command *pCmd = CommandUtils::Parse(ev);
	pCmd->Execute(*this);
	delete pCmd;
}

Page * GameScene::GetCurrentPage()
{
	return &Page_;
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
		Page_ << "You want to move ... where?\n";
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
			Page_ << "You bump your head on the wall. You can't go that way.\n";
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UnknownCommand & cmd)
{
	Page_ << "Sorry, I did not quite get that.\n";
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