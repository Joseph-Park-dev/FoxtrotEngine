#include "FoxtrotEngine/Actors/UIs/ButtonUI.h"
#include "FoxtrotEngine/Scenes/Scene.h"

void ButtonUI::OnMouseHovering()
{
}

void ButtonUI::OnMouseLButtonDown()
{
}

void ButtonUI::OnMouseLButtonUp()
{
}

void ButtonUI::OnMouseLButtonClicked()
{
	if (mFunc != nullptr)
	{
		mFunc(mParam1, mParam2);
	}
	if (mSceneInstance && mSceneFunc)
	{
		((*mSceneInstance).*mSceneFunc)();
	}
}

ButtonUI::ButtonUI(Scene* scene, bool isCamAffected)
	:UIActor(scene, isCamAffected)
	, mFunc(nullptr)
	, mParam1(0)
	, mParam2(0)
	, mSceneInstance(nullptr)
	, mSceneFunc(nullptr)
{
}

ButtonUI::~ButtonUI()
{
}
