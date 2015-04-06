#include <SDLApp.h>
#include <Game.h>
#include <SDL_image.h>
#include <Scene.h>
#include <IntroScene.h>
#include <GameScene.h>
#include <SDL_ttf.h>

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
	if (TTF_Init() < 0)
		throw runtime_error(TTF_GetError());

	initFlags = MIX_INIT_MP3;
	retFlags = Mix_Init(initFlags);
	if (initFlags != retFlags)
		throw runtime_error(Mix_GetError());

	int audio_freq = MIX_DEFAULT_FREQUENCY;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 1;
	int audio_buffers = 1024;
	if (Mix_OpenAudio(audio_freq, audio_format, audio_channels, audio_buffers))
		throw runtime_error(Mix_GetError());

	window_ = NULL;
	renderer_ = NULL;
	currentScene_ = NULL;
}
SDLApp::~SDLApp()
{
	DeleteScene("Game");
	DeleteScene("Intro");
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);

	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void SDLApp::Init(const string &title, int width, int height,
	int flags)
{
	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (window_ == NULL)
		throw runtime_error(SDL_GetError());

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer_ == NULL)
		throw runtime_error(SDL_GetError());
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);

	time_ = SDL_GetTicks();

	IntroScene * intro = new IntroScene();
	intro->Init(renderer_);
	intro->SetName("Intro");
	AddScene(intro);
	GameScene * gameScene = new GameScene();
	gameScene->Init(renderer_);
	gameScene->SetName("Game");
	AddScene(gameScene);
	SetCurrentScene("Intro");
	SDL_initFramerate(&fpsManager);
	SDL_setFramerate(&fpsManager, 30);
}
void SDLApp::Render()
{
	if (currentScene_) currentScene_->Render(renderer_);
	SDL_framerateDelay(&fpsManager);
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