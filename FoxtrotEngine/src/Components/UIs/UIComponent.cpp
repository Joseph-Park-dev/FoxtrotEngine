// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/UIComponent.h"

#include "Actors/Transform.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Managers/KeyInputManager.h"
#include "Managers/UIManager.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
#include "DebugShapes.h"
#include "ResourceSystem/FTShape.h"
#include "CommandHistory.h"
#endif // DEBUG

bool UIComponent::IsMouseHovering()
{
	return mMouseHovering;
}

bool UIComponent::LeftButtonIsDown()
{
	return mLBtnDown;
}

bool UIComponent::LeftButtonIsClicked()
{
	return mLBtnClicked;
}

bool UIComponent::GetIsFocused()
{
	return mIsFocused;
}

bool UIComponent::GetIsAffectedByCamera()
{
	return mIsAffectedByCamera;
}

void UIComponent::SetIsFocused(bool isFocused)
{
	mIsFocused = isFocused;
}

void UIComponent::SetIsAffectedByCamera(bool affected)
{
	mIsAffectedByCamera = affected;
}

void UIComponent::SetMouseHovering(bool hovering)
{
	mMouseHovering = hovering;
}

void UIComponent::CheckMouseHover()
{
	FTVector2 mousePos		= KeyInputManager::GetInstance()->GetMousePosition();
	Transform* transform	= GetOwner()->GetTransform();
	FTVector2 worldPosition = transform->GetWorldPosition();
	FTVector2 scale = mSize * FTVector2(transform->GetScale()) * Camera::GetInstance()->GetPixelsPerUnit();

	if (mIsAffectedByCamera)
	{
		//mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}
	if (worldPosition.x - scale.x/2 <= mousePos.x &&	// Mouse X boundaries
		mousePos.x <= worldPosition.x + scale.x/2 &&

		-worldPosition.y - scale.y/2 <= mousePos.y &&	// Mouse Y boundaries
		mousePos.y <= -worldPosition.y + scale.y/2)
	{
		mMouseHovering = true;
	}
	else
	{
		mMouseHovering = false;
	}
}

void UIComponent::OnMouseHovering()
{}

void UIComponent::OnMouseLButtonDown()
{
	printf("%s", GetName().c_str());
}

void UIComponent::OnMouseLButtonUp()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UIComponent::OnMouseLButtonClicked()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UIComponent::Initialize(FTCore* ftCoreInst)
{
	UIManager::GetInstance()->RegisterUI(this);
#ifdef FOXTROT_EDITOR
	mDebugRect = new FTRectangle;
	mDebugRect->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
#endif // DEBUG
}

void UIComponent::Update(float deltaTime)
{}

void UIComponent::LateUpdate(float deltaTime)
{
	CheckMouseHover();
}

void UIComponent::Render(FoxtrotRenderer* renderer)
{}

UIComponent::UIComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mIsAffectedByCamera(false)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mLBtnClicked(false)
	, mIsFocused(false)
	, mSize(FTVector2(1.f, 1.f))

#ifdef FOXTROT_EDITOR
	, mDebugRect(nullptr)
#endif // FOXTROT_EDITOR
{}

UIComponent::~UIComponent()
{}

#ifdef FOXTROT_EDITOR
void UIComponent::EditorUpdate(float deltaTime)
{
	CheckMouseHover();

	Transform* transform = GetOwner()->GetTransform();
	FTVector3 pos = transform->GetWorldPosition();
	pos = pos * FTVector3(1.f, -1.f, 1.f);
	FTVector3 rot = transform->GetRotation();
	FTVector3 scale = FTVector3(
		transform->GetScale().x * mSize.x,
		transform->GetScale().y * mSize.y,
		1.f
	);

	mDebugRect->Update(
		pos,
		rot,
		scale,
		Camera::GetInstance()
	);
}

void UIComponent::EditorRender(FoxtrotRenderer* renderer)
{
	mDebugRect->Render(renderer);
}

void UIComponent::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize, FLOATMOD_SPEED);
}
#endif // FOXTROT_EDITOR