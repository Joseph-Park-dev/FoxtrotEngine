// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/ButtonUIComponent.h"

#include "Scenes/Scene.h"

void ButtonUIComponent::OnMouseHovering()
{}

void ButtonUIComponent::OnMouseLButtonDown()
{}

void ButtonUIComponent::OnMouseLButtonUp()
{}

void ButtonUIComponent::OnMouseLButtonClicked()
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

void ButtonUIComponent::CloneTo(Actor* actor)
{
	ButtonUIComponent* newComp = DBG_NEW ButtonUIComponent(actor, GetDrawOrder(), GetUpdateOrder());
}

ButtonUIComponent::ButtonUIComponent(Actor* owner, int drawOrder, int updateOrder)
	: UIComponent		(owner, drawOrder, updateOrder)
	, mFunc				(nullptr)
	, mParam1			(0)
	, mParam2			(0)
	, mSceneInstance	(nullptr)
	, mSceneFunc		(nullptr)
{}

ButtonUIComponent::~ButtonUIComponent()
{}