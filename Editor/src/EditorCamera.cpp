// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorCamera.h"

#include "InputSystem/D3D11InputDevice.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "Renderer/FTRectArea.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/EditorShapes.h"
#include "Renderer/FTWindow.h"
#include "Math/FTMath.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"

namespace Editor
{
	using namespace Core;
	using namespace D3D11;
	using namespace Math;

	constexpr float LOOKAT_MODSPEED = 0.01;

	EditorCamera::EditorCamera()
		: Camera()
		, mPanKeyPressed(false)
		, mPanValModSpeed(0.01f)
		, mZoomValModSpeed(0.1f)
		, mDebugRect(DBG_NEW D3D11::FTRectangle)
	{
		// EditorCamera needs to be behind the Camera
		// to let debug rect visible.
		const Math::FTVector3& camPos = Camera::GetInstance()->GetPosition();
		SetPosition(camPos.x, camPos.y, camPos.z - 0.1f);
	}

	EditorCamera::~EditorCamera()
	{
		delete mDebugRect;
		mDebugRect = nullptr;
	}

	D3D11::FTRectangle* EditorCamera::GetDebugRect()
	{
		return mDebugRect;
	}

	void EditorCamera::Initialize(Core::FTWindow* renderWindow, unsigned int pixels, float unit)
	{
		Camera::Initialize(renderWindow, pixels, unit);
		EditorShapes::GetInstance()->SetCameraRect(mDebugRect);
	}

	void EditorCamera::ProcessInput(D3D11InputDevice* inputDevice)
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
				PanLocalXY(Math::FTVector2(delta.x, delta.y));
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

		Math::FTMatrix4 translationMat = Math::FTMatrix4::CreateTranslation(Camera::GetInstance()->GetPosition());
		mDebugRect->UpdateVC(translationMat, this);

		mDebugRect->UpdateGC(this);
		Math::FTVector2 size = Camera::GetInstance()->GetResolution();
		size /= Camera::GetInstance()->GetPixelsPerUnit();
		size /= Camera::GetInstance()->GetZoomFactor();
		mDebugRect->GetGSCData().size = size;
		mDebugRect->SetIsActive(true);
		mDebugRect->UpdatePC();
	}

	void EditorCamera::PanLocalXY(Math::FTVector2 vec2)
	{
		Position().x += vec2.x;
		Position().y += vec2.y;
	}

	void EditorCamera::DisplayMainCameraMenu()
	{
		ImVec2 area = ImVec2(ImGui::GetContentRegionAvail().x, 150.f);
		ImGui::BeginChild("Main Camera", area);
		ImGui::SeparatorText("Main Camera");

		Math::FTVector3& pos = Camera::GetInstance()->Position();
		CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", pos, LOOKAT_MODSPEED);

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
		Scene*							  scene		  = EditorSceneManager::GetInstance()->GetCurrentScene();
		EditorScene*					  editorScene = reinterpret_cast<EditorScene*>(scene);
		FTDS::DynamicArray<Core::Actor*>* editorElems = editorScene->Actors();
		FTDS::String* actorNames					  = DBG_NEW FTDS::String[editorElems->GetSize() + 1];
		actorNames[0].Assign("None");
		static size_t currIdx;

		for (size_t i = 0; i < editorElems->GetSize(); ++i)
			actorNames[i + 1] = editorElems->At(i)->GetName();

		const char* comboPreview = actorNames[currIdx].C_Str();
		if (ImGui::BeginCombo(D3D11::ChunkKey::TARGET_ACTOR, comboPreview))
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

		CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", Camera::GetInstance()->Offset(), LOOKAT_MODSPEED);
		CommandHistory::GetInstance()->UpdateFloatValue("Zoom", Camera::GetInstance()->ZoomFactor());

		if (ImGui::Button("2D"))
		{
			if (GetViewType() == Viewtype::Perspective)
			{
				SetViewType(Viewtype::Orthographic);
				Camera::GetInstance()->SetViewType(Viewtype::Orthographic);
				printf("Orthographic");
			}
			else if (GetViewType() == Viewtype::Orthographic)
			{
				SetViewType(Viewtype::Perspective);
				Camera::GetInstance()->SetViewType(Viewtype::Perspective);
				printf("Perspective");
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
} // namespace Editor