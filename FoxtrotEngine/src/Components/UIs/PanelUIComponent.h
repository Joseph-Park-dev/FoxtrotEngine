// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Detects mouse hovering & clicking.
/// When clicked and dragged, it follows the mouse position.
/// </summary>

#pragma once
#include "Components/UIs/UIComponent.h"

class PanelUIComponent :
	public UIComponent
{
public:
	virtual std::string GetName() const override
	{
		return "PanelUIComponent";
	}

public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;

public:
	 PanelUIComponent(
		Actor* owner, 
		int drawOrder, 
		int updateOrder
	 );
	~PanelUIComponent() override;

private:
	FTVector2 mDragStartPoint;
};