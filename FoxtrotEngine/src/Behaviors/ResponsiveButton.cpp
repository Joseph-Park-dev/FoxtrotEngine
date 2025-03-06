#include "ResponsiveButton.h"

#include <Tweeny/tweeny.h>

#include "Components/UIs/ButtonUI.h"
#include "Actors/Actor.h"

void ResponsiveButton::Setup()
{
	mButton = GetOwner()->GetComponent<ButtonUI>();
	mButton->OnMouseLeftBTNClicked +=
}

void ResponsiveButton::Update(float deltaTime)
{
}

void ResponsiveButton::CloneTo(Actor* actor)
{
}
