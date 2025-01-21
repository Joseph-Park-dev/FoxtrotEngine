// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/UIs/UI.h"

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

bool UI::IsMouseHovering()
{
	return mMouseHovering;
}

bool UI::LeftButtonIsDown()
{
	return mLBtnDown;
}

bool UI::LeftButtonIsClicked()
{
	return mLBtnClicked;
}

bool UI::GetIsFocused()
{
	return mIsFocused;
}

bool UI::GetIsAffectedByCamera()
{
	return mIsAffectedByCamera;
}

void UI::SetIsFocused(bool isFocused)
{
	mIsFocused = isFocused;
}

void UI::SetIsAffectedByCamera(bool affected)
{
	mIsAffectedByCamera = affected;
}

void UI::SetMouseHovering(bool hovering)
{
	mMouseHovering = hovering;
}

void UI::CheckMouseHover()
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

void UI::OnMouseHovering()
{}

void UI::OnMouseLButtonDown()
{
	printf("%s", GetName().c_str());
}

void UI::OnMouseLButtonUp()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UI::OnMouseLButtonClicked()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UI::Initialize(FTCore* ftCoreInst)
{
	UIManager::GetInstance()->RegisterUI(this);
#ifdef FOXTROT_EDITOR
	mDebugRect = DBG_NEW FTRectangle;
	mDebugRect->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
#endif // DEBUG
}

void UI::Update(float deltaTime)
{}

void UI::LateUpdate(float deltaTime)
{
	CheckMouseHover();
}

void UI::Render(FoxtrotRenderer* renderer)
{}

UI::UI(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
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

UI::~UI()
{}

#ifdef FOXTROT_EDITOR
void UI::EditorUpdate(float deltaTime)
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

void UI::EditorRender(FoxtrotRenderer* renderer)
{
	mDebugRect->Render(renderer);
}

void UI::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize, FLOATMOD_SPEED);
}
#endif // FOXTROT_EDITOR