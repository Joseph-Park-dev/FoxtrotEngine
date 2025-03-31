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

EditorCamera::EditorCamera()
	: Camera()
	, mPanKeyPressed(false)
	, mPanValModSpeed(0.01f)
	, mZoomValModSpeed(0.1f)
	, mZoomDelta(0.f)
{

}

EditorCamera::~EditorCamera()
{

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

	if (mPanKeyPressed)
	{
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		delta *= ImVec2(-1, 1) * mPanValModSpeed;
		PanLocalXY(delta);
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
	}
	Zoom();
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
