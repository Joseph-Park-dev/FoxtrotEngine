// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/PanelUI.h"

#include "Managers/KeyInputManager.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Math/FTMath.h"

void PanelUI::OnMouseHovering()
{
	if (LeftButtonIsDown())
		printf("PanelUI - Left button is down");
}

void PanelUI::OnMouseLButtonDown()
{
	mDragStartPoint = MOUSE_POS;
}

void PanelUI::OnMouseLButtonUp()
{}

void PanelUI::CloneTo(Actor * actor)
{
	PanelUI* newComp = DBG_NEW PanelUI(actor, GetDrawOrder(), GetUpdateOrder());
}

PanelUI::PanelUI(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER)
	: UI(owner, drawOrder, updateOrder)
	, mDragStartPoint(FTVector2::Zero)
{}

PanelUI::~PanelUI()
{}