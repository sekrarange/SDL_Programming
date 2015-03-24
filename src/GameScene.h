#ifndef __GameScene_h__
#define __GameScene_h__
#include <Scene.h>

class GameScene : public Scene
{
protected:
	SDL_Texture * cover_;
	SDL_Texture * pages_;
	SDL_Texture * playerTexture_;
	SDL_Rect playerSrcRect;
	SDL_Rect playerDstRect;
public:
	void Init(SDL_Renderer * renderer);
	void Update(float seconds);
	void Render(SDL_Renderer * renderer);
	void OnEvent(SDL_Event & ev);
};


#endif