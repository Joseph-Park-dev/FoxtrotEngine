// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Detects mouse hovering & clicking and executes the registered 
/// operations as a button.
/// </summary>

#pragma once
#include "Components/UIs/UI.h"

#include "delegates/Delegates.h"

class Scene;

typedef void(*BTN_FUNC) (int, int);
typedef void(Scene::*SCENE_MEMFUNC)(void);
typedef void(Actor::*ACTOR_MEMFUNC)(void);

class ButtonUI
	: public UI
{
public:
	virtual FTDS::String GetName() const override
	{
		return "ButtonUI";
	}

public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;
	virtual void OnMouseLButtonClicked() override;

	void SetClickedCallBack(Scene* scene, SCENE_MEMFUNC sceneFunc)
	{
		mSceneInstance = scene;
		mSceneFunc = sceneFunc;
	}

	MulticastDelegate<> OnMouseLeftBTNClicked;

public:
	virtual void CloneTo(Actor* actor) override;

public:
	 ButtonUI(
		Actor* owner,
		int updateOrder = DefaultVal::UPDATE_ORDER
	 );
	~ButtonUI() override;

private:
	BTN_FUNC	  mFunc;
	int			  mParam1;
	int			  mParam2;

	SCENE_MEMFUNC mSceneFunc;
	Scene*		  mSceneInstance;
};