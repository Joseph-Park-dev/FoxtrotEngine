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
#include "ResourceSystem/FTRectangle.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"
#include "Managers/KeyInputManager.h"
#include "Managers/UIManager.h"
#include "Managers/DebugShapes.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
#include "ResourceSystem/FTShape.h"
#include "CommandHistory.h"
#endif // DEBUG

bool UI::IsMouseHovering()
{
	if (mMouseHovering)
		printf("Hovered!\n");
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

FTRectangle* UI::GetInputArea()
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

void UI::SetInputArea(FTRectangle* area)
{
	mInputArea = area;
}

void UI::SetColorID(uint8_t r, uint8_t g, uint8_t b)
{
	mColorID[0] = r;
	mColorID[1] = g;
	mColorID[2] = b;
	mColorID[3] = 255.0f;
	mInputArea->GetPixelConstantData().IndexColor =
		DirectX::SimpleMath::Vector4((float)r / 255, (float)g / 255, (float)b / 255, 1.0);
}

void UI::CheckMouseHover()
{
	if (mIsAffectedByCamera)
	{
		// mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}

	mMouseHovering = CompareColorIDs(mRenderer->GetCursorPosColor());
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
	mInputArea->Initialize(ftCoreInst->GetGameRenderer());
}

void UI::Update(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();
	mInputArea->Update(
		transform->GetWorldPosition(),
		transform->GetRotation(),
		transform->GetScale(),
		Camera::GetInstance());
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
	, mInputArea(DBG_NEW FTRectangle)
	, mRenderer(nullptr)
	, mColorID()
{
}

UI::~UI()
{
}

bool UI::CompareColorIDs(uint8_t* cursorPosCol)
{
	printf("%u, %u, %u, %u\n", cursorPosCol[0], cursorPosCol[1], cursorPosCol[2], cursorPosCol[3]);

	return mColorID[0] == cursorPosCol[0] &&
		mColorID[1] == cursorPosCol[1] &&
		mColorID[2] == cursorPosCol[2] &&
		mColorID[3] == cursorPosCol[3];
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
	this->Update(deltaTime);
}

void UI::EditorRender(FoxtrotRenderer* renderer)
{
	//mInputArea->Render(renderer);
}

void UI::EditorUIUpdate()
{
	mInputArea->UpdateUI();
}
#endif // FOXTROT_EDITOR