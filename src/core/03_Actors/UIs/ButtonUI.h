#pragma once
#include "UIActor.h"

class Scene;

typedef void(*BTN_FUNC) (int, int);
typedef void(Scene::*SCENE_MEMFUNC)(void);
typedef void(Actor::*ACTOR_MEMFUNC)(void);

class ButtonUI
	: public UIActor
{
private:
	BTN_FUNC	  mFunc;
	int			  mParam1;
	int			  mParam2;

	SCENE_MEMFUNC mSceneFunc;
	Scene*		  mSceneInstance;

public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;
	virtual void OnMouseLButtonClicked() override;

	void SetClickedCallBack(BTN_FUNC func, int param1, int param2)
	{
		mFunc = func;
		mParam1 = param1;
		mParam2 = param2;
	}

	void SetClickedCallBack(Scene* scene, SCENE_MEMFUNC sceneFunc)
	{
		mSceneInstance = scene;
		mSceneFunc = sceneFunc;
	}

public:
	ButtonUI(class Scene* scene, bool isCamAffected);
	~ButtonUI();
};

