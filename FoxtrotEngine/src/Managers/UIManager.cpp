// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Managers/UIManager.h"

#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Managers/KeyInputManager.h"
#include "Components/UIs/UI.h"

#ifdef FOXTROT_EDITOR
#include "EditorLayer.h"
#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

UIManager::UIManager()
	: mFocusedUI(nullptr)
{}

UIManager::~UIManager()
{}

void UIManager::RegisterUI(UI* UI)
{
	mUIs.push_back(UI);
}

void UIManager::Update(float deltaTime)
{
	// 1. Identify Focused UI
	// 2. 
	mFocusedUI = GetFocusedUI();
	if (!mFocusedUI)
		return;
	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);
	bool lBtnAway = MOUSE_AWAY(MOUSE::MOUSE_LEFT);
	
	UI* targetUI = GetTargetedUI(mFocusedUI);
	if (targetUI != nullptr)
	{
		targetUI->OnMouseHovering(); // Double check mouse hovering
		if (lBtnTap)
		{
			targetUI->OnMouseLButtonDown();
			targetUI->mLBtnDown = true;
		}
		else if (lBtnAway)
		{
			targetUI->OnMouseLButtonUp();
			if (targetUI->mLBtnDown)
			{
				targetUI->OnMouseLButtonClicked();
			}
			// Uncheck when the left button is released
			targetUI->mLBtnDown = false;
		}
	}
}

UI* UIManager::GetFocusedUI()
{
	//Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
	//std::vector<Actor*>& UI = currScene->GetActorGroup(ActorGroup::UI);

	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);

	UI* focusedUI = mFocusedUI;
	if (!lBtnTap)
		return focusedUI;

	std::vector<UI*>::iterator targetIter= mUIs.end();
	std::vector<UI*>::iterator iter = mUIs.begin();
	for (; iter != mUIs.end(); ++iter)
	{
		if (((UI*)*iter)->IsMouseHovering())
		{
			targetIter = iter;
		}
	}
	// There's no UI focused
	if (targetIter == mUIs.end())
	{
		return nullptr;
	}
	focusedUI = *targetIter;
	//mUIs.erase(targetIter);
	//mUIs.push_back(focusedUI);
	return focusedUI;
}

UI* UIManager::GetTargetedUI(UI* parentUI)
{
	bool lBtnAway = MOUSE_AWAY(MOUSE::MOUSE_LEFT);

	UI* targetUI = nullptr;
	// 1. Including parent UI, inspect all its child objects
	//	  (using BFS utilizing queue DS)
	static std::list<UI*> queue;
	static std::vector<UI*> noneTarget;

	queue.clear();
	noneTarget.clear();
	
	queue.push_back(parentUI);

	while (!queue.empty())
	{
		// 2. Among the overlapped UIs, select the one with higher priority
		UI* ui = queue.front();
		queue.pop_front();

		if (ui->IsMouseHovering())
		{
			if (targetUI != nullptr)
			{
				noneTarget.push_back(targetUI);
			}
			targetUI = ui;
		}
		else
		{
			noneTarget.push_back(ui);
		}

		std::vector<Actor*>& childActors = ui->GetOwner()->GetChildActors();
		for (size_t i = 0; i < childActors.size(); ++i)
		{
			UI* uiComp = childActors[i]->GetComponent<UI>();
			if(uiComp)
				queue.push_back(uiComp);
		}
		if (lBtnAway)
		{
			for (size_t i = 0; i < noneTarget.size(); ++i)
			{
				if (lBtnAway)
				{
					noneTarget[i]->mLBtnDown = false;
				}
			}
		}
	}
	return targetUI;
}

void UIManager::Reset()
{
	mFocusedUI = nullptr;
}