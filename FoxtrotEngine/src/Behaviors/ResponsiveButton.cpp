#include "ResponsiveButton.h"

#include <Tweeny/tweeny.h>

#include "Components/UIs/ButtonUI.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"

void ResponsiveButton::Setup()
{
	mButton = GetOwner()->GetComponent<ButtonUI>();
	mButton->OnMouseLeftBTNClicked.AddRaw(this, & ResponsiveButton::OnMouseClicked);
}

void ResponsiveButton::Update(float deltaTime)
{
	mCurrTime += deltaTime;
	// Used for Fade in,out effect; 
	float mmRatio = 0.f;
	if (mRatio < 0.f)
		mRatio = 0;
	if (mRatio > 1.f)
		mRatio = 1;
	mRatio = mCurrTime / mDuration;
}

void ResponsiveButton::CloneTo(Actor* actor)
{
}

ResponsiveButton::ResponsiveButton(Actor* owner, int updateOrder)
	: FTBehavior(owner, updateOrder)
{
}

ResponsiveButton::~ResponsiveButton()
{
}

void ResponsiveButton::OnMouseHover()
{
}

void ResponsiveButton::OnMouseEscape()
{
}

void ResponsiveButton::OnMouseClicked()
{
	printf("Hello!");
	FTVector3 scale = mButton->GetOwner()->GetTransform()->GetScale();
	auto tween = tweeny::from(scale.x).to(0.f);
	tween.step(mRatio);
}

void ResponsiveButton::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
}

void ResponsiveButton::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}
