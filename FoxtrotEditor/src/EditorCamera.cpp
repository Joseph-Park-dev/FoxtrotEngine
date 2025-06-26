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
#include <Dynamic/DynamicArray.h>

#include <EditorLayer.h>
#include <EditorSceneManager.h>

EditorCamera::EditorCamera()
	: Camera()
	, mPanKeyPressed(false)
	, mPanValModSpeed(0.01f)
	, mZoomValModSpeed(0.1f)
	, mZoomDelta(0.f)
	, mDebugRect(DBG_NEW FTRectangle)
{
	Position() = Vector3(0.0f, 0.0f, 5.0f);
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

	mPanKeyPressed = ImGui::IsMouseDragging(ImGuiMouseButton_Middle);

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
	std::vector<EditorElement*> editorElems;
	editorElems				 = EditorSceneManager::GetInstance()->GetEditorScene()->GetEditorElements();
	FTDS::String* actorNames = DBG_NEW FTDS::String[editorElems.size() + 1];
	actorNames[0]			 = "None";
	static size_t currIdx;

	for (size_t i = 0; i < editorElems.size(); ++i)
		actorNames[i + 1] = editorElems.at(i)->GetName();

	const char* comboPreview = actorNames[currIdx].C_Str();
	if (ImGui::BeginCombo(ChunkKey::TARGET_ACTOR, comboPreview))
	{
		for (size_t i = 0; i < editorElems.size() + 1; ++i)
		{
			if (ImGui::Selectable(actorNames[i].C_Str()))
			{
				currIdx = i;
				if (currIdx == 0)
				Camera::GetInstance()->SetTargetActor(nullptr);
				else
				{
					EditorElement* actor = editorScene->FindEditorElement(actorNames[currIdx], nullptr);
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

	if (ImGui::Button("2D"))
	{
		FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
		if (GetViewType() == Viewtype::Perspective)
		{
			SetViewType(Viewtype::Orthographic);
			LogString("Orthographic");
		}
		else if (GetViewType() == Viewtype::Orthographic)
		{
			SetViewType(Viewtype::Perspective);
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
