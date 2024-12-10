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
{
}

PanelUIComponent::PanelUIComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER)
	: UIComponent(owner, drawOrder, updateOrder)
	, mDragStartPoint(FTVector2::Zero)
{
}

PanelUIComponent::~PanelUIComponent()
{
}
