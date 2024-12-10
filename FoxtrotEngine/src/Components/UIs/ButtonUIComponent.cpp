#include "Components/UIs/ButtonUIComponent.h"

#include "Scenes/Scene.h"

void ButtonUIComponent::OnMouseHovering()
{}

void ButtonUIComponent::OnMouseLButtonDown()
{
}

void ButtonUIComponent::OnMouseLButtonUp()
{
}

void ButtonUIComponent::OnMouseLButtonClicked()
{
	if (mFunc != nullptr)
	{
		mFunc(mParam1, mParam2);
	}
	if (mSceneInstance && mSceneFunc)
	{
		((*mSceneInstance).*mSceneFunc)();
	}
	printf("Clicked!\n");
}

ButtonUIComponent::ButtonUIComponent(Actor* owner, int drawOrder, int updateOrder)
	: UIComponent(owner, drawOrder, updateOrder)
	, mFunc(nullptr)
	, mParam1(0)
	, mParam2(0)
	, mSceneInstance(nullptr)
	, mSceneFunc(nullptr)
{
}

ButtonUIComponent::~ButtonUIComponent()
{
}