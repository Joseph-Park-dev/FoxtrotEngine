// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorCamera.h"

#include "InputSystem/IInputDevice.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "Renderer/FTRectArea.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/EditorShapes.h"
#include "Renderer/IWindow.h"
#include "FTMath.h"
#include "Plugin/GetFunc.h"
#include "FileSystem/DLLPath.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"

namespace Editor
{
	using namespace Core;
	using namespace D3D11;
	using namespace Math;

	constexpr float LOOKAT_MODSPEED = 0.01;

	EditorCamera::EditorCamera(Core::CameraData* data)
		: mData(data)
		, mPanKeyPressed(false)
		, mPanValModSpeed(0.01f)
		, mZoomValModSpeed(0.1f)
		, mDebugRect(nullptr)
	{
		//// EditorCamera needs to be behind the Camera
		//// to let debug rect visible.
		// Math::FTVector3& camPos = Camera::GetInstance()->Position();
		// camPos.z -= 0.1f;
		// SetPosition(camPos);

		using GET_RECT_FUNC = D3D11::FTRectangle* (*)();
		mDebugRect			= GetFunc<GET_RECT_FUNC>(DLLPath::D3D11_EDITOR, D3D11::ProcName::CREATE_FT_RECTANGLE)();
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

	float& EditorCamera::ZoomFactor()
	{
		return mData->ZoomFactor;
	}

	void EditorCamera::Initialize(Core::IWindow* renderWindow, unsigned int pixels, float unit)
	{
		FTVector2 size = renderWindow->GetRenderArea()->GetSize();
		mData->ResX	   = static_cast<unsigned int>(size.x);
		mData->ResY	   = static_cast<unsigned int>(size.y);
		InitializePixelsPerUnit(pixels, unit);
		EditorShapes::GetInstance()->SetCameraRect(mDebugRect);
	}

	void EditorCamera::ProcessInput(IInputDevice* inputDevice)
	{
		if (0 < inputDevice->GetMouseWheelDelta())
			ZoomFactor() += mZoomValModSpeed;
		else if (inputDevice->GetMouseWheelDelta() < 0)
			ZoomFactor() -= mZoomValModSpeed;
	}

	void EditorCamera::Update(Core::ICamera* gameCam)
	{
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

		Math::FTMatrix4 translationMat = Math::FTMatrix4::CreateTranslation(gameCam->GetPosition());
		mDebugRect->UpdateVC(translationMat, this);

		mDebugRect->UpdateGC(this);
		Math::FTVector2 size = gameCam->GetResolution();
		size *= gameCam->GetUnitsPerPixel();
		size /= gameCam->GetZoomFactor();
		mDebugRect->GetGSCData().size = size;
		mDebugRect->SetIsActive(true);
		mDebugRect->UpdatePC();
	}

	void EditorCamera::PanLocalXY(Math::FTVector2 vec2)
	{
		mData->Position.x += vec2.x;
		mData->Position.y += vec2.y;
	}

	void EditorCamera::DisplayGameCameraMenu(Core::ICamera* gameCam)
	{
		ImVec2 area = ImVec2(ImGui::GetContentRegionAvail().x, 150.f);
		ImGui::BeginChild("Game Camera", area);
		ImGui::SeparatorText("Game Camera");

		CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", gameCam->Data()->Position, LOOKAT_MODSPEED);

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
		Scene*									   scene	   = EditorSceneManager::GetInstance()->GetCurrentScene();
		EditorScene*							   editorScene = reinterpret_cast<EditorScene*>(scene);
		Common::FTDS::DynamicArray<Core::IActor*>* editorElems = editorScene->Actors();
		Common::FTDS::String* actorNames					   = DBG_NEW Common::FTDS::String[editorElems->GetSize() + 1];
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
						gameCam->SetTargetActor(nullptr);
					else
					{
						Core::IActor* actor =
							editorScene->FindActor(actorNames[currIdx], nullptr);
						gameCam->SetTargetActor(actor);
					}
				}
			}
			ImGui::EndCombo();
		}
		delete[] actorNames;

		CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", gameCam->Data()->Offset, LOOKAT_MODSPEED);
		CommandHistory::GetInstance()->UpdateFloatValue("Zoom", gameCam->ZoomFactor());

		if (ImGui::Button("2D"))
		{
			if (GetViewType() == Viewtype::Perspective)
			{
				SetViewType(Viewtype::Orthographic);
				gameCam->SetViewType(Viewtype::Orthographic);
				printf("Orthographic");
			}
			else if (GetViewType() == Viewtype::Orthographic)
			{
				SetViewType(Viewtype::Perspective);
				gameCam->SetViewType(Viewtype::Perspective);
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