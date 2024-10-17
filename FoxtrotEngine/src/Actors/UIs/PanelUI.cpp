#include "Actors/UIs/PanelUI.h"

#include "Managers/KeyInputManager.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Math/FTMath.h"

void PanelUI::OnMouseHovering()
{
	if (LeftButtonIsDown())
	{
		/*FTVector2 diff = MOUSE_POS - mDragStartPoint;
		FTVector2 curPos = GetTransform()->GetWorldPosition();
		curPos += diff;
		GetTransform()->SetWorldPosition(curPos);
		for (size_t i = 0; i < GetChildActors().size(); ++i)
		{
			FTVector2 curChildPos = ((UIActor*)GetChildActors()[i])->GetTransform()->GetWorldPosition();
			curChildPos += diff;
			((UIActor*)GetChildActors()[i])->GetTransform()->SetWorldPosition(curChildPos);
		}

		mDragStartPoint = MOUSE_POS;*/
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
	, mDragStartPoint(FTVector2::Zero)
{
}

PanelUI::PanelUI(Actor* origin, Scene* scene)
	: UIActor(origin, scene)
	, mDragStartPoint(FTVector2::Zero)
{
}

PanelUI::~PanelUI()
{
}
