#ifndef __GameScene_h__
#define __GameScene_h__
#include <Scene.h>
#include <Commands.h>
#include <Game.h>

class GameScene : public Scene, public CommandHandler
{
protected:
	Game * gameInstance;
	SDL_Texture * cover_;
	SDL_Texture * pages_;
	SDL_Texture * playerTexture_;
	SDL_Rect playerSrcRect;
	SDL_Rect playerDstRect;
public:
	GameScene();
	void Init(SDL_Renderer * renderer);
	void Update(float seconds);
	void Render(SDL_Renderer * renderer);
	void OnEvent(SDL_Event & ev);

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