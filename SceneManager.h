#pragma once
#include "SingletonMacro.h"
#include <SDL2/SDL.h>

class Scene;
enum class SCENE_TYPE;

class SceneManager
{
	SINGLETON(SceneManager);

public:
	void	SwitchScene(SCENE_TYPE sceneType);
	Scene*	GetCurrScene() { return mPCurrScene; }

private:
	Scene*  mArrScene[(unsigned int)SCENE_TYPE::END];
	Scene*  mPCurrScene;

public:
	void	Init();
	void	ProcessInput(class KeyInputManager* keyInputManager);
	void	Update(float deltaTime);
	void	Lateupdate(float deltaTime);
	void	Render(SDL_Renderer* renderer);
	void	ProcessEvent();

#ifdef _DEBUG
public:
	void	EditorUpdate(float deltaTime);
	void	EditorLateUpdate(float deltaTime);
#endif
};