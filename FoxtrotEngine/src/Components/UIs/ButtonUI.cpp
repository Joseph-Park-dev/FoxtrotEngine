// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/ButtonUI.h"

#include <tuple>
#include "delegates/Delegates.h"

#include "Scenes/Scene.h"
#include "Renderer/FTRectArea.h"
#include "ResourceSystem/FTRectangle.h"

void ButtonUI::OnMouseHovering()
{
}

void ButtonUI::OnMouseLButtonDown()
{}

void ButtonUI::OnMouseLButtonUp()
{}

void ButtonUI::OnMouseLButtonClicked()
{
	if (mFunc != nullptr)
	{
		mFunc(mParam1, mParam2);
	}
	if (mSceneInstance && mSceneFunc)
	{
		((*mSceneInstance).*mSceneFunc)();
	}

	OnMouseLeftBTNClicked.Broadcast();
}

void ButtonUI::CloneTo(Actor* actor)
{
	ButtonUI* newComp = DBG_NEW ButtonUI(actor, GetUpdateOrder());
	newComp->SetDebugShape(GetDebugShape());
	this->RectArea()->CloneTo(newComp->RectArea());
}

ButtonUI::ButtonUI(Actor* owner, int updateOrder)
	: UI		(owner, updateOrder)
	, mFunc				(nullptr)
	, mParam1			(0)
	, mParam2			(0)
	, mSceneInstance	(nullptr)
	, mSceneFunc		(nullptr)
{}

ButtonUI::~ButtonUI()
{
	OnMouseLeftBTNClicked.Clear();
}