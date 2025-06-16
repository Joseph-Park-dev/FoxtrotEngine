// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorCamera.h"

#include <directxtk/SimpleMath.h>

#include "InputSystem/FTInputDevice.h"
#include "EditorLayer.h"
#include "ResourceSystem/FTRectangle.h"

EditorCamera::EditorCamera()
	: Camera()
	, mPanKeyPressed(false)
	, mPanValModSpeed(0.01f)
	, mZoomValModSpeed(0.1f)
	, mZoomDelta(0.f)
	, mDebugRect(DBG_NEW FTRectangle)
{
}

EditorCamera::~EditorCamera()
{
	delete mDebugRect;
	mDebugRect = nullptr;
}

FTRectangle* EditorCamera::GetDebugRect()
{
	return mDebugRect;
}

void EditorCamera::ProcessInput(FTInputDevice* inputDevice)
{
	if (0 < inputDevice->GetMouseWheelDelta())
		mZoomDelta = mZoomValModSpeed;
	else if (inputDevice->GetMouseWheelDelta() < 0)
		mZoomDelta = -mZoomValModSpeed;
	else
		mZoomDelta = 0.f;
}

void EditorCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	mPanKeyPressed = ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsMouseDragging(ImGuiMouseButton_Middle);
	mRotationKeyPressed = ImGui::IsMouseDragging(ImGuiMouseButton_Middle);

	if (mRotationKeyPressed)
	{
	}

	ImVec2 viewportMin = EditorLayer::GetInstance()->GetSceneViewportPos();
	ImVec2 viewportMax = viewportMin + EditorLayer::GetInstance()->GetSceneViewportSize();
	if (EditorLayer::GetInstance()->CursorOnViewport())
	{
		if (mPanKeyPressed)
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
			delta *= ImVec2(-1, 1) * mPanValModSpeed;
			PanLocalXY(delta);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
		}
		Zoom();
	}

	/*Camera* cam = Camera::GetInstance();
	mDebugRect->UpdateVC(
		cam->GetEyePos(), 
		FTVector3::Zero, 
		FTVector3(1.0f, 1.0f, 1.0f), 
		this);

	FTVector2 resRatio = FTVector2(cam->GetAspectRatio(), 1.f);
	resRatio *= cam->GetPixelsPerUnit();
	mDebugRect->GetGSCData().size = resRatio.GetD3Vec2();
	mDebugRect->GetPixelConstantData().IsActive = true;*/
}

void EditorCamera::PanLocalXY(ImVec2 vec2)
{
	Position().x += vec2.x;
	Position().y += vec2.y;
}

void EditorCamera::Zoom()
{
	Position().z += mZoomDelta;
}