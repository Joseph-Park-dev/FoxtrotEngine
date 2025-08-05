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
#include "InputSystem/FTInputDevice.h"
#include "Managers/UIManager.h"
#include "Managers/DebugShapes.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
	#include "ResourceSystem/FTShape.h"
	#include "CommandHistory.h"
	#include "EditorCamera.h"
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

FTRectangle* UI::GetDebugShape() const
{
	return mDBGShape;
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

void UI::SetDebugShape(FTRectangle* area)
{
	mDBGShape = area;
}

FTRectArea* UI::RectArea()
{
	return mRectArea;
}

// void UI::SetColorID(uint8_t r, uint8_t g, uint8_t b)
//{
//	mColorID[0] = r;
//	mColorID[1] = g;
//	mColorID[2] = b;
//	mColorID[3] = 255.0f;
//	//mInputArea->GetPixelConstantData().IndexColor =
//	//	DirectX::SimpleMath::Vector4((float)r / 255, (float)g / 255, (float)b / 255, 1.0);
// }

void UI::CheckMouseHover(FTVector2 mousePos)
{
	if (mIsAffectedByCamera)
	{
		// mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}
	mMouseHovering = mRectArea->Overlaps(mousePos);
}

void UI::OnMouseHovering()
{
}

void UI::OnMouseLButtonDown()
{
	printf("%s", GetName().C_Str());
}

void UI::OnMouseLButtonUp()
{
	// SDL_Log("%ls", GetName().C_Str());
}

void UI::OnMouseLButtonClicked()
{
	// SDL_Log("%ls", GetName().C_Str());
}

void UI::Initialize(FTCore* ftCoreInst)
{
	UIManager::GetInstance()->RegisterUI(this);
	mRenderer = ftCoreInst->GetGameRenderer();
	mDBGShape->Initialize(mRenderer);
	DebugShapes::GetInstance()->AddShape(mDBGShape);
}

void UI::ProcessInput(FTInputDevice* inputDevice)
{
	CheckMouseHover(inputDevice->MOUSE_POS());
}

void UI::Render(FoxtrotRenderer* renderer)
{
	Camera::GetInstance()->SetViewType(Viewtype::Orthographic);
	UpdateDebugShape(Camera::GetInstance());
	//Camera::GetInstance()->SetViewType(Viewtype::Perspective);
}

UI::UI(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mIsAffectedByCamera(false)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mLBtnClicked(false)
	, mIsFocused(false)
	, mDBGShape(DBG_NEW FTRectangle)
	, mRectArea(DBG_NEW FTRectArea)
	, mRenderer(nullptr)
//, mColorID()
{
}

UI::~UI()
{
	DebugShapes::GetInstance()->RemoveShape(mDBGShape);
	delete mRectArea;
}

// bool UI::CompareColorIDs(uint8_t* cursorPosCol)
//{
//	return mColorID[0] == cursorPosCol[0] &&
//		mColorID[1] == cursorPosCol[1] &&
//		mColorID[2] == cursorPosCol[2] &&
//		mColorID[3] == cursorPosCol[3];
// }

void UI::UpdateDebugShape(Camera* camInst)
{
	if (!mDBGShape)
		return;

	if (!mDBGShape->GetIsActive())
		return;

	Transform* transform = GetOwner()->GetTransform();
	FTVector2  center	 = mRectArea->GetCenter();
	center /= static_cast<float>(camInst->GetPixelsPerUnit());
	Matrix translationMat = Matrix::CreateTranslation(center.x, center.y, 0.0f);
	Matrix modelMat		  = translationMat * transform->GetMatrixWorld();

	mDBGShape->UpdateVC(modelMat, camInst);

	mDBGShape->GetGSCData().size.x = mRectArea->GetSize().x / camInst->GetPixelsPerUnit();
	mDBGShape->GetGSCData().size.y = mRectArea->GetSize().y / camInst->GetPixelsPerUnit();
	mDBGShape->UpdateGC(camInst);

	mDBGShape->UpdatePC();
}

void UI::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	mRectArea->SaveProperties(ofs);
	mDBGShape->SaveProperties(ofs);
}

void UI::LoadProperties(std::ifstream& ifs)
{
	mDBGShape->LoadProperties(ifs);
	mRectArea->LoadProperties(ifs);
	Component::LoadProperties(ifs);

	Transform* transform = GetOwner()->GetTransform();
	FTVector2  screenPos = transform->GetScreenPosition(Camera::GetInstance());
	mRectArea->Set(screenPos, mRectArea->GetSize());
}

#ifdef FOXTROT_EDITOR
void UI::EditorRender(FoxtrotRenderer* renderer)
{
	Camera::GetInstance()->SetViewType(Viewtype::Orthographic);
	//UpdateDebugShape(Camera::GetInstance());
	//Camera::GetInstance()->SetViewType(Viewtype::Perspective);
}

void UI::EditorUIUpdate()
{
	mDBGShape->UpdateUI();
	mRectArea->UpdateUI();
}
#endif // FOXTROT_EDITOR