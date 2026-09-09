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

	/// @brief Converts a screen-space position into world coordinates using the camera transforms.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts a screen-space position into world coordinates using the camera transforms.
	Math::FTVector3 EditorCamera::ScreenToWorld(const Math::FTVector2& screenPos)
	{
		return mData->ScreenToWorld(screenPos);
	}

	/// @brief Projects a world-space position into screen coordinates.
	/// @param worldPos Position in world coordinates.
	/// @return Current world to screen.
	Math::FTVector2 EditorCamera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return mData->WorldToScreen(worldPos);
	}

	/// @brief Converts screen coordinates to normalized device coordinates.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts screen coordinates to normalized device coordinates.
	Math::FTVector2 EditorCamera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		return mData->ScreenToNDC(screenPos);
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void EditorCamera::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void EditorCamera::LoadProperties(std::ifstream& ifs)
	{
		mData->LoadProperties(ifs);
	}

	/// @brief Builds the controls for configuring the game camera.
	/// @param gameCam Camera used to render the game scene.
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

	/// @brief Builds the controls for configuring the editor camera.
	void EditorCamera::DisplayEditorCameraMenu()
	{
		ImGui::BeginChild("Editor Camera");
		ImGui::SeparatorText("Editor Camera");

		FTVector3 pos = this->GetPosition();
		CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", pos, LOOKAT_MODSPEED);
		this->SetPosition(pos);

		ImGui::EndChild();
	}

	/// @brief Exposes the stored payload for consumers of this resource or container.
	/// @return Borrowed access to the data.
	D3D11::CameraData* EditorCamera::Data()
	{
		return mData;
	}

	/// @brief Returns the position used by this editor camera.
	/// @return Borrowed access to the position.
	const Math::FTVector3& EditorCamera::GetPosition() const
	{
		return mData->Position;
	}

	/// @brief Returns the view matrix used by this editor camera.
	/// @param outViewMat Receives the view matrix.
	void EditorCamera::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		mData->GetViewMatrix(outViewMat);
	}

	/// @brief Returns the projection matrix used by this editor camera.
	/// @param outProjMat Receives the projection matrix.
	void EditorCamera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		mData->GetProjectionMatrix(outProjMat);
	}

	/// @brief Returns the view type used by this editor camera.
	/// @return Current view type.
	const Core::Viewtype EditorCamera::GetViewType()
	{
		return mData->ViewType;
	}

	/// @brief Returns the proj fovangle y used by this editor camera.
	/// @return Current proj fovangle y.
	const float EditorCamera::GetProjFOVAngleY()
	{
		return mData->ProjFOVAngleY;
	}

	/// @brief Returns the aspect ratio used by this editor camera.
	/// @return Current aspect ratio.
	const float EditorCamera::GetAspectRatio()
	{
		return mData->Aspect;
	}

	/// @brief Returns the units per pixel used by this editor camera.
	/// @return Current units per pixel.
	const float EditorCamera::GetUnitsPerPixel()
	{
		return mData->UnitsPerPixel;
	}

	/// @brief Returns the near z used by this editor camera.
	/// @return Current near z.
	const float EditorCamera::GetNearZ()
	{
		return mData->NearZ;
	}

	/// @brief Returns the far z used by this editor camera.
	/// @return Current far z.
	const float EditorCamera::GetFarZ()
	{
		return mData->FarZ;
	}

	/// @brief Returns the off set used by this editor camera.
	/// @return Borrowed access to the off set.
	const Math::FTVector3& EditorCamera::GetOffSet() const
	{
		return mData->Offset;
	}

	/// @brief Returns the zoom factor used by this editor camera.
	/// @return Current zoom factor.
	const float EditorCamera::GetZoomFactor() const
	{
		return mData->ZoomFactor;
	}

	/// @brief Returns the resolution used by this editor camera.
	/// @return Current resolution.
	Math::FTVector2 EditorCamera::GetResolution() const
	{
		return mData->GetResolution();
	}

	/// @brief Returns the debug rect used by this editor camera.
	/// @return Borrowed access to the debug rect.
	D3D11::FTRectangle* EditorCamera::GetDebugRect()
	{
		return mDebugRect;
	}

	/// @brief Applies a multiplicative change to the editor camera zoom.
	/// @return Applies a multiplicative change to the editor camera zoom.
	float& EditorCamera::ZoomFactor()
	{
		return mData->ZoomFactor;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderWindow Window associated with the render target.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param unit Unit conversion factor.
	void EditorCamera::Initialize(Core::IWindow* renderWindow, unsigned int pixels, float unit)
	{
		FTVector2 size = renderWindow->GetRenderArea()->GetSize();
		mData->ResX	   = static_cast<unsigned int>(size.x);
		mData->ResY	   = static_cast<unsigned int>(size.y);
		InitializePixelsPerUnit(pixels, unit);
		EditorShapes::GetInstance()->SetCameraRect(mDebugRect);
	}

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	/// @param inputDevice Device exposing the current frame's input state.
	void EditorCamera::ProcessInput(IInputDevice* inputDevice)
	{
		if (0 < inputDevice->GetMouseWheelDelta())
			ZoomFactor() += mZoomValModSpeed;
		else if (inputDevice->GetMouseWheelDelta() < 0)
			ZoomFactor() -= mZoomValModSpeed;
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param gameCam Camera used to render the game scene.
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

	/// @brief Initializes camera state used for editor navigation.
	/// @note Initializes the :EditorCamera base or delegates to its constructor.
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

	/// @brief Releases the resources managed by this instance during destruction.
	EditorCamera::~EditorCamera()
	{
		delete mData;
		delete mDebugRect;
		mDebugRect = nullptr;
	}

	/// @brief Establishes the conversion from engine world units to pixels.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param units World-space distance or unit count.
	void EditorCamera::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		mData->InitializePixelsPerUnit(pixels, units);
	}

	/// @brief Moves the editor camera along its local horizontal and vertical axes.
	/// @param vec2 Two-dimensional vector read or written.
	void EditorCamera::PanLocalXY(Math::FTVector2 vec2)
	{
		mData->Position.x += vec2.x;
		mData->Position.y += vec2.y;
	}
} // namespace Editor
namespace Editor {
/// @brief Updates the position used by subsequent operations.
/// @param position Replacement position.
void EditorCamera::SetPosition(const Math::FTVector3& position) { mData->Position = position; }
/// @brief Updates the view type used by subsequent operations.
/// @param type Replacement view type.
void EditorCamera::SetViewType(Core::Viewtype type) { mData->ViewType = type; }
/// @brief Updates the target actor used by subsequent operations.
/// @param actor Replacement target actor.
void EditorCamera::SetTargetActor(Core::IActor* actor) { mData->Target = actor; }
/// @brief Updates the offset used by subsequent operations.
/// @param offset Replacement offset.
void EditorCamera::SetOffset(Math::FTVector3 offset) { mData->Offset = offset; }
}
