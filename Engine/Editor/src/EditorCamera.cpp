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

	constexpr float LOOKAT_MODSPEED = 0.01;

	Math::FTVector3 EditorCamera::ScreenToWorld(const Math::FTVector2& screenPos)
	{
		FTVector2 ndc	  = ScreenToNDC(screenPos);
		FTVector3 clipPos = FTVector3(ndc.x, ndc.y, 0.0f);

		FTMatrix4 view = FTMatrix4::Identity;
		FTMatrix4 proj = FTMatrix4::Identity;

		GetViewMatrix(view);
		GetProjectionMatrix(proj);

		FTMatrix4 viewProj = view * proj;
		viewProj.Invert();

		return FTVector3::Transform(clipPos, viewProj, 1.0f);
	}

	Math::FTVector2 EditorCamera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return Math::FTVector2();
	}

	Math::FTVector2 EditorCamera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		FTVector2 renderSize = mData->GetResolution();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	void EditorCamera::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::CAMERA_DATA);
		if (mData->Target)
			Common::FileIOHelper::SaveString(ofs, Core::ChunkKey::TARGET_ACTOR, mData->Target->GetNameRef());
		else
			Common::FileIOHelper::SaveString(ofs, Core::ChunkKey::TARGET_ACTOR, Common::ChunkKey::NullVal::NULL_OBJECT);
		Common::FileIOHelper::SaveVector3(ofs, Core::ChunkKey::CAM_POSITION, mData->Position);
		Common::FileIOHelper::SaveVector3(ofs, Core::ChunkKey::CAM_OFFSET, mData->Offset);
		Common::FileIOHelper::SaveFloat(ofs, Core::ChunkKey::CAM_ZOOM, mData->ZoomFactor);
		Common::FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::CAMERA_DATA);
	}

	void EditorCamera::LoadProperties(std::ifstream& ifs)
	{
#include "Plugin/GetFunc.h"
		Common::FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::CAMERA_DATA);
		Common::FileIOHelper::LoadFloat(ifs, mData->ZoomFactor);
		Common::FileIOHelper::LoadVector3(ifs, mData->Offset);

		Common::FileIOHelper::LoadVector3(ifs, mData->Position);
		Common::FTDS::String targetName = {};
		Common::FileIOHelper::LoadBasicString(ifs, targetName);

#ifdef FOXTROT_EDITOR
		if (!mData->Target)
		{
			using FIND_ACTOR = Core::IActor* (*)(Common::FTDS::String&, Core::IActor*);
			mData->Target	 = GetFunc<FIND_ACTOR>(Plugin::Name::CORE_EDITOR, ProcNames::FIND_ACTOR)(targetName, nullptr);
		}
#else
		if (targetName.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
			mTarget = manager->GetCurrentScene()->FindActor(targetName);
#endif // FOXTROT_EDITOR
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
		if (ImGui::BeginCombo(Core::ChunkKey::TARGET_ACTOR, comboPreview))
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

	void EditorCamera::PanLocalXY(Math::FTVector2 vec2)
	{
		mData->Position.x += vec2.x;
		mData->Position.y += vec2.y;
	}
} // namespace Editor