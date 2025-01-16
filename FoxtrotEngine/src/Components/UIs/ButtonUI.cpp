// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/ButtonUI.h"

#include "Scenes/Scene.h"

void ButtonUI::OnMouseHovering()
{}

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
	printf("Clicked!\n");
}

void ButtonUI::CloneTo(Actor* actor)
{
	ButtonUI* newComp = DBG_NEW ButtonUI(actor, GetDrawOrder(), GetUpdateOrder());
}

ButtonUI::ButtonUI(Actor* owner, int drawOrder, int updateOrder)
	: UI		(owner, drawOrder, updateOrder)
	, mFunc				(nullptr)
	, mParam1			(0)
	, mParam2			(0)
	, mSceneInstance	(nullptr)
	, mSceneFunc		(nullptr)
{}

ButtonUI::~ButtonUI()
{}