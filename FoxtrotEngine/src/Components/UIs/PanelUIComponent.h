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
	virtual ~PanelUIComponent() override;

private:
	FTVector2 mDragStartPoint;
};