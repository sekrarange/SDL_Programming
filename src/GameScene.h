#ifndef __GameScene_h__
#define __GameScene_h__
#include <Scene.h>
#include <Commands.h>
#include <Game.h>
#include <Page.h>
#include <SDL_mixer.h>
const int NUM_PAGE_ANIM_FRAMES = 6;

enum GameState {fadeIn, play};

class GameScene : public Scene, public CommandHandler
{
protected:
	Game * gameInstance;
	SDL_Texture * cover_;
	SDL_Texture * pages_[NUM_PAGE_ANIM_FRAMES];
	SDL_Texture * playerTexture_;
	TTF_Font * font_;
	SDL_Rect playerSrcRect;
	SDL_Rect playerDstRect;
	static Page Page_;
	SDL_Rect PageRect;
	GameState state;
	int alpha;
	void LoadFont(const std::string &filename);
	Mix_Music * bgMusic;
	Mix_Chunk * stepSound;
	Mix_Chunk * wallSound;
	Mix_Chunk * screamSound;
	Mix_Chunk * pageFlip;

	int pageToShow_{ 0 };
	bool pageTurning_{ false };
	bool pageTurningBack_{ false };
	float pageFrame_{ 0 };

public:
	Page * titlePage_;
	Page * endPage_;

	GameScene();
	virtual ~GameScene();
	void Init(SDL_Renderer * renderer);
	void Update(float seconds);
	void Render(SDL_Renderer * renderer);
	void OnEvent(SDL_Event & ev);
	static Page * GetCurrentPage();
	void PlayDeathSound();
	Mix_Chunk * squishSound;

	void Execute(UseCommand & cmd);
	void Execute(ExamineCommand & cmd);
	void Execute(QuitCommand & cmd);
	void Execute(MoveCommand & cmd);
	void Execute(UnknownCommand & cmd);
	void Execute(TakeCommand & cmd);
	void Execute(DropCommand & cmd);
	void Execute(InventoryCommand & cmd);
	void Execute(LookCommand & cmd);
	void Execute(NullCommand & cmd);
};


#endif