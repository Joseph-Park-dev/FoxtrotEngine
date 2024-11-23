#pragma once
#include "Core/SingletonMacro.h"
#include "Scenes/SceneType.h"

class Scene;
class FoxtrotRenderer;

class SceneManager
{
	SINGLETON(SceneManager)

public:
	void	SwitchScene(SCENE_TYPE sceneType);
	Scene*	GetCurrentScene()	{ return mPCurrScene; }

public:
	virtual void Initialize();
	virtual void ProcessInput(class KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	virtual void Lateupdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);
	virtual void ProcessEvent();

			void DeleteAll();

private:
	Scene*  mArrScene[(unsigned int)SCENE_TYPE::END];
	Scene*  mPCurrScene;
};