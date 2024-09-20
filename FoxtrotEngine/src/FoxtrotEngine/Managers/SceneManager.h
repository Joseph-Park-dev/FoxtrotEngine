#pragma once
#include "FoxtrotEngine/Core/SingletonMacro.h"
#include "FoxtrotEngine/Scenes/SceneType.h"

class Scene;
class FoxtrotRenderer;

class SceneManager
{
	SINGLETON(SceneManager)

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
	void	Render(FoxtrotRenderer* renderer);
	void	ProcessEvent();

#ifdef _DEBUG
public:
	void	EditorProcessInput(KeyInputManager* keyInputManager);
	void	EditorUpdate(float deltaTime);
	void	EditorLateUpdate(float deltaTime);
	void	EditorRender(FoxtrotRenderer* renderer);
#endif
};