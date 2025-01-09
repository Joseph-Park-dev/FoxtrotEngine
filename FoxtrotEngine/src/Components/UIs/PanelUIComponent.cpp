// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/PanelUIComponent.h"

#include "Managers/KeyInputManager.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Math/FTMath.h"

void PanelUIComponent::OnMouseHovering()
{
	if (LeftButtonIsDown())
		printf("PanelUIComponent - Left button is down");
}

void PanelUIComponent::OnMouseLButtonDown()
{
	mDragStartPoint = MOUSE_POS;
}

void PanelUIComponent::OnMouseLButtonUp()
{}

void PanelUIComponent::CloneTo(Actor * actor)
{
	PanelUIComponent* newComp = DBG_NEW PanelUIComponent(actor, GetDrawOrder(), GetUpdateOrder());
}

PanelUIComponent::PanelUIComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER)
	: UIComponent(owner, drawOrder, updateOrder)
	, mDragStartPoint(FTVector2::Zero)
{}

PanelUIComponent::~PanelUIComponent()
{}