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
#include "Components/UIs/UI.h"

class PanelUI :
	public UI
{
public:
	virtual std::string GetName() const override
	{
		return "PanelUI";
	}

public:
	virtual void OnMouseHovering()		override;
	virtual void OnMouseLButtonDown()	override;
	virtual void OnMouseLButtonUp()		override;

public:
	virtual void CloneTo(Actor* actor)	override;

public:
	 PanelUI(
		Actor* owner, 
		int updateOrder
	 );
	~PanelUI() override;

private:
	FTVector2 mDragStartPoint;
};