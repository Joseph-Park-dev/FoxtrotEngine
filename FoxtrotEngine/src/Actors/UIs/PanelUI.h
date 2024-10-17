#pragma once
#include "Actors/UIs/UIActor.h"

class PanelUI :
	public UIActor
{
private:
	FTVector2 mDragStartPoint;
public:
	virtual void OnMouseHovering() override;
	virtual void OnMouseLButtonDown() override;
	virtual void OnMouseLButtonUp() override;


public:
	PanelUI(Scene* scene, bool isCamAffected);
	PanelUI(Actor* origin, Scene* scene);
	virtual ~PanelUI() override; 
};