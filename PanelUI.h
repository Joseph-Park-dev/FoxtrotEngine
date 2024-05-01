#pragma once
#include "UIActor.h"

class Actor;

class PanelUI :
	public UIActor
{
private:
	Vector2 mDragStartPoint;
public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;

	CLONE(PanelUI);

public:
	PanelUI(class Scene* scene, bool isCamAffected);
	PanelUI(Actor* origin);
	virtual ~PanelUI() override; 
};