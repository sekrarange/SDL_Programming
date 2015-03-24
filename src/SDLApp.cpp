#include <SDLApp.h>
#include <Game.h>
#include <SDL_image.h>
#include <Scene.h>
#include <IntroScene.h>

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
	playerTexture_ = NULL;
	currentScene_ = NULL;
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
	if (renderer_ == NULL)
		throw runtime_error(SDL_GetError());
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);

	cover_ = IMG_LoadTexture(renderer_, "./res/cover.png");
	if (cover_ == NULL) throw runtime_error(SDL_GetError());
	pages_ = IMG_LoadTexture(renderer_, "./res/pages.png");
	if (pages_ == NULL) throw runtime_error(SDL_GetError());
	playerTexture_ = IMG_LoadTexture(renderer_, "./res/Player0.png");
	if (playerTexture_ == NULL) throw runtime_error(SDL_GetError());

	playerSrcRect.x = 1;
	playerSrcRect.y = 1;
	playerSrcRect.w = 14;
	playerSrcRect.h = 16;

	int windowSizeW, windowSizeH;
	SDL_GetWindowSize(window_, &windowSizeW, &windowSizeH);
	playerDstRect.x = windowSizeW / 2 - playerSrcRect.w / 2;
	playerDstRect.y = windowSizeH / 2 - playerSrcRect.h / 2;
	playerDstRect.w = playerSrcRect.w * 2;
	playerDstRect.h = playerSrcRect.h * 2;

	time_ = SDL_GetTicks();

	IntroScene * intro = new IntroScene();
	intro->Init(renderer_);
	intro->SetName("Intro");
	AddScene(intro);
	SetCurrentScene("Intro");
}
void SDLApp::Render()
{
	//SDL_RenderCopy(renderer_, cover_, NULL, NULL);
	//SDL_RenderCopy(renderer_, pages_, NULL, NULL);
	//SDL_RenderCopy(renderer_, playerTexture_, &playerSrcRect, &playerDstRect);

	if (currentScene_) currentScene_->Render(renderer_);
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
			currentScene_->OnEvent(ev);

			Command *pCmd = CommandUtils::Parse(ev);
			pCmd->Execute(*Game::GetInstance());
			delete pCmd;
			break;
		}
	}
}

void SDLApp::AddScene(Scene * scene)
{
	if(scenes_.find(scene->GetName()) != scenes_.end())
	{
		ostringstream ss("Scene with name '");
		ss << scene->GetName() << "' already exists!";
		throw runtime_error(ss.str());
	}
	scenes_[scene->GetName()] = scene;
}

void SDLApp::DeleteScene(const std::string & name)
{
	auto it = scenes_.find(name);
	if (it != scenes_.end())
	{
		delete it->second;
		scenes_.erase(it);
	}
	else
	{
		ostringstream ss("No scene with name '");
		ss << name << "' exists!";
		throw runtime_error(ss.str());
	}
}

void SDLApp::SetCurrentScene(const std::string & name)
{
	auto it = scenes_.find(name);
	if (it != scenes_.end())
	{
		currentScene_ = it->second;
	}
}

Scene * SDLApp::GetCurrentScene()
{
	return currentScene_;
}

SDL_Window	* SDLApp::GetWindow()
{
	return window_;
}


void SDLApp::Update()
{
	const int MIN_ALLOWED_TIME_STEP_MS = 5;

	Uint32 slice = SDL_GetTicks() - time_;
	if (slice >= MIN_ALLOWED_TIME_STEP_MS)
	{
		if (currentScene_)
		{
			float seconds = (float)slice * 0.001f;
			currentScene_->Update(seconds);
			time_ = SDL_GetTicks();
		}
	}
}