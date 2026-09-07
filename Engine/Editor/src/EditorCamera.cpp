// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorCamera.h"

#include "Renderer/Camera.h"
#include "Renderer/CameraData.h"
#include "InputSystem/IInputDevice.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "Renderer/FTRectArea.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/EditorShapes.h"
#include "Renderer/IWindow.h"
#include "FTMath.h"
#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"
#include "FileSystem/DLLPath.h"
#include "FileSystem/FileIOHelper.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"

namespace Editor
{
	using namespace Core;
	using namespace D3D11;
	using namespace Math;

	constexpr float LOOKAT_MODSPEED = 0.01f;

	Math::FTVector3 EditorCamera::ScreenToWorld(const Math::FTVector2& screenPos)
	{
		return mData->ScreenToWorld(screenPos);
	}

	Math::FTVector2 EditorCamera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return mData->WorldToScreen(worldPos);
	}

	Math::FTVector2 EditorCamera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		return mData->ScreenToNDC(screenPos);
	}

	void EditorCamera::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	void EditorCamera::LoadProperties(std::ifstream& ifs)
	{
		mData->LoadProperties(ifs);
	}

	void EditorCamera::DisplayGameCameraMenu(Core::ICamera* gameCam)
	{
		ImVec2 area = ImVec2(ImGui::GetContentRegionAvail().x, 150.f);
		ImGui::BeginChild("Game Camera", area);
		ImGui::SeparatorText("Game Camera");

		D3D11::CameraData* gameData = static_cast<D3D11::Camera*>(gameCam)->Data();
		CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", gameData->Position, LOOKAT_MODSPEED);

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
		if (ImGui::BeginCombo(Graphics::ChunkKey::TARGET_ACTOR, comboPreview))
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

		CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", gameData->Offset, LOOKAT_MODSPEED);
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

	D3D11::CameraData* EditorCamera::Data()
	{
		return mData;
	}

	const Math::FTVector3& EditorCamera::GetPosition() const
	{
		return mData->Position;
	}

	void EditorCamera::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		mData->GetViewMatrix(outViewMat);
	}

	void EditorCamera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		mData->GetProjectionMatrix(outProjMat);
	}

	const Core::Viewtype EditorCamera::GetViewType()
	{
		return mData->ViewType;
	}

	const float EditorCamera::GetProjFOVAngleY()
	{
		return mData->ProjFOVAngleY;
	}

	const float EditorCamera::GetAspectRatio()
	{
		return mData->Aspect;
	}

	const float EditorCamera::GetUnitsPerPixel()
	{
		return mData->UnitsPerPixel;
	}

	const float EditorCamera::GetNearZ()
	{
		return mData->NearZ;
	}

	const float EditorCamera::GetFarZ()
	{
		return mData->FarZ;
	}

	const Math::FTVector3& EditorCamera::GetOffSet() const
	{
		return mData->Offset;
	}

	const float EditorCamera::GetZoomFactor() const
	{
		return mData->ZoomFactor;
	}

	Math::FTVector2 EditorCamera::GetResolution() const
	{
		return mData->GetResolution();
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

	EditorCamera::EditorCamera()
		: mData(DBG_NEW CameraData)
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
		mDebugRect			= Core::GetFunc<GET_RECT_FUNC>(Common::DLLPath::D3D11_EDITOR, D3D11::ProcName::CREATE_FT_RECTANGLE)();
	}

	EditorCamera::~EditorCamera()
	{
		delete mData;
		delete mDebugRect;
		mDebugRect = nullptr;
	}

	void EditorCamera::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		mData->InitializePixelsPerUnit(pixels, units);
	}

	void EditorCamera::PanLocalXY(Math::FTVector2 vec2)
	{
		mData->Position.x += vec2.x;
		mData->Position.y += vec2.y;
	}
} // namespace Editor
namespace Editor {
void EditorCamera::SetPosition(const Math::FTVector3& position) { mData->Position = position; }
void EditorCamera::SetViewType(Core::Viewtype type) { mData->ViewType = type; }
void EditorCamera::SetTargetActor(Core::IActor* actor) { mData->Target = actor; }
void EditorCamera::SetOffset(Math::FTVector3 offset) { mData->Offset = offset; }
}
