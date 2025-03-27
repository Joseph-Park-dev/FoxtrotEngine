// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Managers/UIManager.h"

#include <directxtk/SimpleMath.h>

#include "Math/Random.h"
#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "InputSystem/FTInputDevice.h"
#include "Components/UIs/UI.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

UIManager::UIManager()
	: mFocusedUI(nullptr)
{
}

UIManager::~UIManager()
{
}

void UIManager::RegisterUI(UI* UI)
{
	UI->SetColorID(
		Random::GetIntRange(0, 255),
		Random::GetIntRange(0, 255),
		Random::GetIntRange(0, 255));
	mUIs.push_back(UI);
}

void UIManager::Update(float deltaTime, FTInputDevice* inputDevice)
{
	// 1. Identify Focused UI
	// 2.
	mFocusedUI = GetFocusedUI();
	if (!mFocusedUI)
		return;
	bool lBtnTap  = inputDevice->MOUSE_TAP(MOUSE::MOUSE_LEFT);
	bool lBtnAway = inputDevice->MOUSE_AWAY(MOUSE::MOUSE_LEFT);

	// UI* targetUI = GetTargetedUI(mFocusedUI);
	UI* targetUI = mFocusedUI;
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
	std::list<UI*>			   hoveredUI;
	std::vector<UI*>::iterator iter = mUIs.begin();
	for (; iter != mUIs.end(); ++iter)
	{
		if ((*iter)->IsMouseHovering())
			hoveredUI.push_back(*iter);
	}

	// There's no UI focused
	if (hoveredUI.empty())
		return nullptr;

	UI* focusedUI = nullptr;
	focusedUI	  = hoveredUI.back();
	hoveredUI.pop_back();

	while (!hoveredUI.empty())
	{
		UI*	  ui		= hoveredUI.back();
		float depth		= ui->GetOwner()->GetTransform()->GetWorldPosition().z;
		float currDepth = focusedUI->GetOwner()->GetTransform()->GetWorldPosition().z;
		if (currDepth < depth)
			focusedUI = ui;
		hoveredUI.pop_back();
	}

	return focusedUI;
}

UI* UIManager::GetTargetedUI(UI* parentUI, FTInputDevice* inputDevice)
{
	bool lBtnAway = inputDevice->MOUSE_AWAY(MOUSE::MOUSE_LEFT);

	UI* targetUI = nullptr;
	// 1. Including parent UI, inspect all its child objects
	//	  (using BFS utilizing queue DS)
	static std::list<UI*>	queue;
	static std::vector<UI*> noneTarget;

	queue.clear();
	noneTarget.clear();

	queue.push_back(parentUI);

	while (!queue.empty())
	{
		// 2. Among the overlapped UIs, select the one with higher priority
		UI* ui = queue.front();
		queue.pop_front();

		if (ui)
		{
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
			if (!childActors.empty())
			{
				for (size_t i = 0; i < childActors.size(); ++i)
				{
					UI* uiComp = childActors[i]->GetComponent<UI>();
					if (uiComp)
						queue.push_back(uiComp);
				}
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
	}
	return targetUI;
}

void UIManager::Reset()
{
	mUIs.clear();
	mFocusedUI = nullptr;
}