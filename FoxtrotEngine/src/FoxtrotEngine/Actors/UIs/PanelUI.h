#pragma once
#include "FoxtrotEngine/Actors/UIs/UIActor.h"

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
	PanelUI(class Scene* scene, bool isCamAffected);
	PanelUI(Actor* origin);
	virtual ~PanelUI() override; 
};