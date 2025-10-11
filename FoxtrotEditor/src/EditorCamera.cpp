// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorCamera.h"

#include <directxtk/SimpleMath.h>

#include <InputSystem/FTInputDevice.h>
#include <ResourceSystem/FTRectangle.h>
#include <Renderer/FTRectArea.h>
#include <Dynamic/DynamicArray.h>
#include <Managers/DebugShapes.h>
#include <WindowSystem/FTWindow.h>

#include <EditorLayer.h>
#include <EditorSceneManager.h>

EditorCamera::EditorCamera()
	: Camera()
	, mPanKeyPressed(false)
	, mPanValModSpeed(0.01f)
	, mZoomValModSpeed(0.1f)
	, mDebugRect(DBG_NEW FTRectangle)
{
	// EditorCamera needs to be behind the Camera
	// to let debug rect visible.
	Position().z = Camera::GetInstance()->GetPosition().z - 0.1f;
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

void EditorCamera::Initialize(FTWindow* renderWindow, UINT pixels, float unit)
{
	Camera::Initialize(renderWindow, pixels, unit);
	DebugShapes::GetInstance()->SetCameraRect(mDebugRect);
}

void EditorCamera::ProcessInput(FTInputDevice* inputDevice)
{
	if (0 < inputDevice->GetMouseWheelDelta())
		ZoomFactor() += mZoomValModSpeed;
	else if (inputDevice->GetMouseWheelDelta() < 0)
		ZoomFactor() -= mZoomValModSpeed;
}

void EditorCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	mPanKeyPressed = ImGui::IsMouseDragging(ImGuiMouseButton_Middle);
	if (EditorLayer::GetInstance()->CursorOnViewport())
	{
		if (mPanKeyPressed)
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
			delta *= ImVec2(-1, 1) * mPanValModSpeed;
			PanLocalXY(delta);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
		}
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

	Matrix translationMat = Matrix::CreateTranslation(Camera::GetInstance()->GetPosition().GetDXVec3());
	mDebugRect->UpdateVC(translationMat, this);

	mDebugRect->UpdateGC(this);
	Vector2 size = Camera::GetInstance()->GetResolution().GetD3Vec2();
	size /= Camera::GetInstance()->GetPixelsPerUnit();
	size /= Camera::GetInstance()->GetZoomFactor();
	mDebugRect->GetGSCData().size = size;
	mDebugRect->SetIsActive(true);
	mDebugRect->UpdatePC();
}

void EditorCamera::PanLocalXY(ImVec2 vec2)
{
	Position().x += vec2.x;
	Position().y += vec2.y;
}

void EditorCamera::DisplayMainCameraMenu()
{
	ImVec2 area = ImVec2(ImGui::GetContentRegionAvail().x, 150.f);
	ImGui::BeginChild("Main Camera", area);
	ImGui::SeparatorText("Main Camera");

	FTVector3 pos = Camera::GetInstance()->GetPosition();
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", pos, LOOKAT_MODSPEED);
	Camera::GetInstance()->SetPosition(pos);

	/*float yaw	= mYaw;
	float pitch = mPitch;*/

	// CommandHistory::GetInstance()->UpdateFloatValue("Look-At Yaw", yaw, LOOKAT_MODSPEED);
	// CommandHistory::GetInstance()->UpdateFloatValue("Look-At Pitch", mPitch, LOOKAT_MODSPEED);

	/*if (yaw != mYaw || pitch != mPitch)
	{
		mYaw = yaw; mPitch = pitch;
		UpdateViewDirections();
	}*/

	// Set Target
	EditorScene*				editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
	FTDS::DynamicArray<Actor*>* editorElems = editorScene->Actors();
	FTDS::String* actorNames = DBG_NEW FTDS::String[editorElems->GetSize() + 1];
	actorNames[0].Assign("None");
	static size_t currIdx;

	for (size_t i = 0; i < editorElems->GetSize(); ++i)
		actorNames[i + 1] = editorElems->At(i)->GetName();

	const char* comboPreview = actorNames[currIdx].C_Str();
	if (ImGui::BeginCombo(ChunkKey::TARGET_ACTOR, comboPreview))
	{
		for (size_t i = 0; i < editorElems->GetSize() + 1; ++i)
		{
			if (ImGui::Selectable(actorNames[i].C_Str()))
			{
				currIdx = i;
				if (currIdx == 0)
					Camera::GetInstance()->SetTargetActor(nullptr);
				else
				{
					Actor* actor =
						editorScene->FindActor(actorNames[currIdx], nullptr);
					Camera::GetInstance()->SetTargetActor(actor);
				}
			}
		}
		ImGui::EndCombo();
	}
	delete[] actorNames;

	FTVector3 offset = Camera::GetInstance()->GetOffSet();
	CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", offset, LOOKAT_MODSPEED);
	Camera::GetInstance()->SetOffset(offset);

	CommandHistory::GetInstance()->UpdateFloatValue("Zoom", Camera::GetInstance()->ZoomFactor());

	if (ImGui::Button("2D"))
	{
		FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
		if (GetViewType() == Viewtype::Perspective)
		{
			SetViewType(Viewtype::Orthographic);
			Camera::GetInstance()->SetViewType(Viewtype::Orthographic);
			LogString("Orthographic");
		}
		else if (GetViewType() == Viewtype::Orthographic)
		{
			SetViewType(Viewtype::Perspective);
			Camera::GetInstance()->SetViewType(Viewtype::Perspective);
			LogString("Perspective");
		}
	}

	ImGui::EndChild();
}

void EditorCamera::DisplayEditorCameraMenu()
{
	ImGui::BeginChild("Editor Camera");
	ImGui::SeparatorText("Editor Camera");

	FTVector3 pos = this->GetPosition();
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", pos, LOOKAT_MODSPEED);
	this->SetPosition(pos);

	ImGui::EndChild();
}
