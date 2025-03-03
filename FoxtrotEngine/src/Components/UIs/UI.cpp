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
#include "Renderer/FTRect.h"
#include "Managers/KeyInputManager.h"
#include "Managers/UIManager.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Renderer/Camera.h"

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

FTRect* UI::GetInputArea()
{
	return mInputArea;
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

void UI::SetInputArea(FTRect* rect)
{
	mInputArea = rect;
}

void UI::CheckMouseHover()
{
	if (mIsAffectedByCamera)
	{
		// mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}

	LogVector2(MOUSE_POS);
	FTRect rect;
	FTVector2 ndcPos = mRenderer->GetRenderResolution() / 2 + mInputArea->GetCenter() * Camera::GetInstance()->GetPixelsPerUnit();
	FTVector2 ndcSize = mInputArea->GetSize() * 1 / Camera::GetInstance()->GetPixelsPerUnit();
	rect.Set(ndcPos, ndcSize);

	if (rect.Overlaps(MOUSE_POS))
		mMouseHovering = true;
	else
		mMouseHovering = false;
}

void UI::OnMouseHovering()
{
}

void UI::OnMouseLButtonDown()
{
	printf("%s", GetName().c_str());
}

void UI::OnMouseLButtonUp()
{
	// SDL_Log("%ls", GetName().c_str());
}

void UI::OnMouseLButtonClicked()
{
	// SDL_Log("%ls", GetName().c_str());
}

void UI::Initialize(FTCore* ftCoreInst)
{
	UIManager::GetInstance()->RegisterUI(this);
	mRenderer = ftCoreInst->GetGameRenderer();
#ifdef FOXTROT_EDITOR
	mDebugRect = DBG_NEW FTRectangle;
	mDebugRect->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
#endif // DEBUG
}

void UI::Update(float deltaTime)
{
}

void UI::LateUpdate(float deltaTime)
{
	CheckMouseHover();
}

void UI::Render(FoxtrotRenderer* renderer)
{
}

UI::UI(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mIsAffectedByCamera(false)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mLBtnClicked(false)
	, mIsFocused(false)
	, mInputArea(new FTRect())
	, mRenderer(nullptr)

#ifdef FOXTROT_EDITOR
	, mDebugRect(nullptr)
#endif // FOXTROT_EDITOR
{
}

UI::~UI()
{
	delete mInputArea;
}

void UI::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	mInputArea->SaveProperties(ofs);
}

void UI::LoadProperties(std::ifstream& ifs)
{
	mInputArea->LoadProperties(ifs);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void UI::EditorUpdate(float deltaTime)
{
	// CheckMouseHover();

	FTVector2 pos	= mInputArea->GetCenter();
	FTVector3 rot	= GetOwner()->GetTransform()->GetRotation();
	FTVector3 scale = FTVector3(
		mInputArea->GetSize().x / Camera::GetInstance()->GetPixelsPerUnit(),
		mInputArea->GetSize().y / Camera::GetInstance()->GetPixelsPerUnit(),
		1.f);

	mDebugRect->Update(
		pos,
		rot,
		scale,
		Camera::GetInstance());
}

void UI::EditorRender(FoxtrotRenderer* renderer)
{
	mDebugRect->Render(renderer);
}

void UI::EditorUIUpdate()
{
	mInputArea->UpdateUI();
}
#endif // FOXTROT_EDITOR