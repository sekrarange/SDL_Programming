#include <GameScene.h>
#include <SDL_image.h>
#include <stdexcept>
#include <Room.h>
#include <SDL2_gfxPrimitives.h>
#include <tinyxml2.h>
#include <SDL_mixer.h>
#define current_room (*(Game::GetInstance()->GetCurrentRoom()))

using namespace std;
using namespace tinyxml2;

Page  GameScene::Page_ = Page();

GameScene::GameScene()
{
	gameInstance = NULL;
	cover_ = NULL;
	playerTexture_ = NULL;
	font_ = NULL;
	bgMusic = NULL;
	stepSound = NULL;
	wallSound = NULL;
	screamSound = NULL;
	squishSound = NULL;
	pageFlip = NULL;
}
GameScene::~GameScene()
{
	for (auto & e:pages_)
	{ 
		SDL_DestroyTexture(e);
	}
	delete titlePage_;
	delete endPage_;
	Mix_FreeChunk(pageFlip);
	Mix_FreeChunk(squishSound);
	Mix_FreeChunk(screamSound);
	Mix_FreeChunk(wallSound);
	Mix_FreeChunk(stepSound);
	Mix_FreeMusic(bgMusic);
	TTF_CloseFont(font_);
}

void GameScene::Init(SDL_Renderer * renderer)
{
	gameInstance = Game::GetInstance();
	cover_ = IMG_LoadTexture(renderer, "./res/cover.png");
	if (cover_ == NULL) throw runtime_error(IMG_GetError());

	for (int i = 0; i < NUM_PAGE_ANIM_FRAMES; i++)
	{
		ostringstream ss;
		ss << "res/book/page0" << i + 1 << ".png";

		pages_[i] = IMG_LoadTexture(renderer, ss.str().c_str());
		if (pages_[i] == NULL) throw runtime_error(IMG_GetError());
	}
	playerTexture_ = IMG_LoadTexture(renderer, "./res/Player0.png");
	if (playerTexture_ == NULL) throw runtime_error(IMG_GetError());

	playerSrcRect.x = 1;
	playerSrcRect.y = 1;
	playerSrcRect.w = 14;
	playerSrcRect.h = 16;

	playerDstRect.x = WINDOW_WIDTH * 3 / 4 - playerSrcRect.w / 2;
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

	titlePage_ = new Page();
	titlePage_->Init(renderer, 425, 560);
	titlePage_->_pageFont = font_;
	titlePage_->fillMode_ = FillMode::Head;
	titlePage_->_justification = Justification::Center;
	*titlePage_ << "Quick Escape\n";
	titlePage_->SetDirty(true);

	endPage_ = new Page();
	endPage_->Init(renderer, 425, 560);
	endPage_->_pageFont = font_;
	endPage_->fillMode_ = FillMode::Head;
	endPage_->_justification = Justification::Center;
	*endPage_ << "The End\n";
	endPage_->SetDirty(true);

	state = fadeIn;
	alpha = 255;

	bgMusic = Mix_LoadMUS("res/sound/Southern Pastures - I. Homestead.mp3");
	if (bgMusic == NULL)
		throw runtime_error(Mix_GetError());

	stepSound = Mix_LoadWAV("res/sound/stepstone_1.wav");
	if (stepSound == NULL)
		throw runtime_error(Mix_GetError());

	wallSound = Mix_LoadWAV("res/sound/Jump 3.wav");
	if (wallSound == NULL)
		throw runtime_error(Mix_GetError());

	screamSound = Mix_LoadWAV("res/sound/3yell5.wav");
	if (screamSound == NULL)
		throw runtime_error(Mix_GetError());

	squishSound = Mix_LoadWAV("res/sound/squish.wav");
	if (squishSound == NULL)
		throw runtime_error(Mix_GetError());

	pageFlip = Mix_LoadWAV("res/sound/page-flip-12.wav");
	if (pageFlip == NULL)
		throw runtime_error(Mix_GetError());
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
		alpha -= 120 * seconds;
		if (alpha <= 0)
		{
			alpha = 0;
			state = play;
		}
		if (!Mix_PlayingMusic())
			if (Mix_FadeInMusic(bgMusic, -1, 5000) == -1)
				throw runtime_error(Mix_GetError());
	}
	if (pageTurning_)
	{
		pageFrame_ += seconds*3.0f;
		if (pageFrame_ > 5.9f)
		{
			pageTurning_ = false;
			pageFrame_ = 0.0f;
			pageToShow_++;
		}
	}
	if (pageTurningBack_)
	{
		pageFrame_ -= seconds*3.0f;
		if (pageFrame_ < 0.0f)
		{
			pageTurningBack_ = false;
			pageFrame_ = 0.0f;
			pageToShow_--;
		}
	}
}

void GameScene::Render(SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, cover_, NULL, NULL);
	SDL_Rect d = { 10, 35, 980, 650 };
	SDL_RenderCopy(renderer, pages_[0], NULL, &d);
	SDL_RenderCopy(renderer, pages_[(int)pageFrame_], NULL, &d);
	
	if (Page_.IsDirty())
	{
		Page_.Compose(font_);
		Page_.RenderContent(renderer);
		Page_.SetDirty(false);
	}

	if (titlePage_->IsDirty())
	{
		titlePage_->Compose(font_);
		titlePage_->RenderContent(renderer);
		titlePage_->SetDirty(false);
	}

	if (endPage_->IsDirty())
	{
		endPage_->Compose(font_);
		endPage_->RenderContent(renderer);
		endPage_->SetDirty(false);
	}
	if (!pageTurning_ && !pageTurningBack_)
	{
		if (pageToShow_ == 0)
		{
			SDL_RenderCopy(renderer, titlePage_->_pageTexture, NULL, &PageRect);
		}
		else if (pageToShow_ == 1)
		{
			SDL_RenderCopy(renderer, playerTexture_, &playerSrcRect, &playerDstRect);
			SDL_RenderCopy(renderer, Page_._pageTexture, NULL, &PageRect);
		}
		else if (pageToShow_ == 2)
		{
			SDL_RenderCopy(renderer, endPage_->_pageTexture, NULL, &PageRect);
		}
	}
	else
	{
		SDL_Texture * currentTexture = NULL;
		SDL_Texture * nextTexture = NULL;
		if (pageTurning_)
		{
			switch (pageToShow_)
			{
			case 0:
				nextTexture = Page_._pageTexture;
				currentTexture = titlePage_->_pageTexture;
				break;
			case 1:
				nextTexture = endPage_->_pageTexture;
				currentTexture = Page_._pageTexture;
				break;
			}
		}
		else if (pageTurningBack_)
		{
			switch (pageToShow_)
			{
			case 1:
				nextTexture = Page_._pageTexture;
				currentTexture = titlePage_->_pageTexture;
				break;
			case 2:
				nextTexture = endPage_->_pageTexture;
				currentTexture = Page_._pageTexture;
				break;
			}
		}
		SDL_Rect currPage = { 0, 0, 470, 630 };
		SDL_Rect nextClip = { 0, 0, 1, 630 };
		SDL_Rect nextPage = PageRect;
		switch ((int)pageFrame_)
		{
		case 4:
			currPage.w = 240;
			nextClip.x = 260;
			nextClip.w = 257;
			nextPage.x = 265;
			break;
		case 5:
			currPage.w = 66;
			nextClip.x = 90;
			nextClip.w = 400;
			nextPage.x = 93;
			break;
		}

		SDL_RenderSetClipRect(renderer, &currPage);
		SDL_RenderCopy(renderer, currentTexture, NULL, &PageRect);
		SDL_RenderSetClipRect(renderer, &nextClip);
		SDL_RenderCopy(renderer, nextTexture, NULL, &nextPage);

		SDL_RenderSetClipRect(renderer, NULL);
	}
	if (state == fadeIn)
		boxRGBA(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 255, 255, 255, (Uint8)alpha);
}

void GameScene::PlayDeathSound()
{
	int chan;
	chan = Mix_PlayChannel(-1, screamSound, 0);
	while (Mix_Playing(chan));
	Mix_PlayChannel(-1, squishSound, 0);
}

void GameScene::OnEvent(SDL_Event & ev)
{
	Command *pCmd = NULL;
	if (ev.type == SDL_KEYDOWN)
	{
		if (ev.key.keysym.sym == SDLK_SPACE)
		{
			if (pageToShow_ == 2 && pageTurningBack_ == false)
			{
				pCmd = new QuitCommand();
				pCmd->Execute(*this);
			}
			else if (!pageTurning_ && !pageTurningBack_)
			{
				pageTurning_ = true;
				Mix_PlayChannel(-1, pageFlip, 0);
			}
		}
		else if (ev.key.keysym.sym == SDLK_LALT)
		{
			if (!pageTurning_ && !pageTurningBack_ && pageToShow_ != 0)
			{
				pageTurningBack_ = true;
				pageFrame_ = 5.9f;
				Mix_PlayChannel(-1, pageFlip, 0);
			}
		}
		else if (ev.key.keysym.sym == SDLK_ESCAPE)
		{
			pCmd = new QuitCommand();
			pCmd->Execute(*this);
		}
	}
	if (pageToShow_ == 1)
	{
		pCmd = CommandUtils::Parse(ev);
		pCmd->Execute(*this);
	}
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
	Mix_FadeOutMusic(2000);
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
			Mix_PlayChannel(-1, stepSound, 0);
			if (current_room.HasProperty("deadly") && current_room.GetProperty("deadly").As<bool>())
			{
				PlayDeathSound();
				Mix_FadeOutMusic(2000);
			}
		}
		else
		{
			Page_ << "You bump your head on the wall. You can't go that way.\n";
			Mix_PlayChannel(-1, wallSound, 0);
		}
	}
	Page_.SetDirty(true);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UnknownCommand & cmd)
{
	Page_ << "Sorry, I did not quite get that.\n";
	Page_.SetDirty(true);
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