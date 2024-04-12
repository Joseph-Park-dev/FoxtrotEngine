#pragma once
#include "UIActor.h"

class Scene;

typedef void(*BTN_FUNC) (Uint32, Uint32);
typedef void(Scene::*SCENE_MEMFUNC)(void);
typedef void(Actor::*ACTOR_MEMFUNC)(void);

class ButtonUI
	: public UIActor
{
private:
	BTN_FUNC	  mFunc;
	Uint32		  mParam1;
	Uint32		  mParam2;

	SCENE_MEMFUNC mSceneFunc;
	Scene*		  mSceneInstance;

public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;
	virtual void OnMouseLButtonClicked() override;

	void SetClickedCallBack(BTN_FUNC func, Uint32 param1, Uint32 param2)
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
	CLONE(ButtonUI);
};

