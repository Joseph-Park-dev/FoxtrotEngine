#include "PanelUI.h"
#include "KeyInputManager.h"
#include "Transform.h"
#include "Actor.h"

void PanelUI::OnMouseHovering()
{
	if (LeftButtonIsDown())
	{
		Vector2 diff = MOUSE_POS - mDragStartPoint;
		Vector2 curPos = GetTransform()->GetWorldPosition();
		curPos += diff;
		GetTransform()->SetWorldPosition(curPos);
		for (size_t i = 0; i < GetChildActors().size(); ++i)
		{
			Vector2 curChildPos = ((UIActor*)GetChildActors()[i])->GetTransform()->GetWorldPosition();
			curChildPos += diff;
			((UIActor*)GetChildActors()[i])->GetTransform()->SetWorldPosition(curChildPos);
		}

		mDragStartPoint = MOUSE_POS;
	}
}

void PanelUI::OnMouseLButtonDown()
{
	mDragStartPoint = MOUSE_POS;
}

void PanelUI::OnMouseLButtonUp()
{
}

PanelUI::PanelUI(Scene* scene, bool isCamAffected)
	: UIActor(scene, isCamAffected)
	, mDragStartPoint(Vector2::Zero)
{
}

PanelUI::PanelUI(Actor* origin)
	: UIActor(origin)
	, mDragStartPoint(Vector2::Zero)
{
}

PanelUI::~PanelUI()
{
}
