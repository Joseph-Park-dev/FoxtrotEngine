// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Additional camera used to render the editor scene, & navigate
/// through it.
/// </summary>

#pragma once
#include "Renderer/ICamera.h"
#include "Renderer/CameraData.h"
#include "Core/InterfaceAliases.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>


namespace D3D11
{
	class FTRectangle;
} // namespace D3D11

namespace Editor
{
	class EditorCamera :
		public Graphics::ICamera
	{
	public:
		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		/// @brief Converts a screen-space position into world coordinates using the camera transforms.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts a screen-space position into world coordinates using the camera transforms.
		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) override;
		/// @brief Projects a world-space position into screen coordinates.
		/// @param worldPos Position in world coordinates.
		/// @return Current world to screen.
		virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const override;
		/// @brief Converts screen coordinates to normalized device coordinates.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts screen coordinates to normalized device coordinates.
		virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const override;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Builds the controls for configuring the game camera.
		/// @param gameCam Camera used to render the game scene.
		void DisplayGameCameraMenu(Core::ICamera* gameCam);
		/// @brief Builds the controls for configuring the editor camera.
		void DisplayEditorCameraMenu();

	public:
		/// @brief Exposes the stored payload for consumers of this resource or container.
		/// @return Borrowed access to the data.
		virtual D3D11::CameraData* Data();

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		/// @brief Returns the position used by this editor camera.
		/// @return Borrowed access to the position.
		virtual const Math::FTVector3& GetPosition() const override;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// @brief Returns the view matrix used by this editor camera.
		/// @param outViewMat Receives the view matrix.
		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) override;
		/// @brief Returns the projection matrix used by this editor camera.
		/// @param outProjMat Receives the projection matrix.
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) override;

	public:
		/// @brief Returns the view type used by this editor camera.
		/// @return Current view type.
		const Core::Viewtype GetViewType() override;
		/// @brief Returns the proj fovangle y used by this editor camera.
		/// @return Current proj fovangle y.
		const float			 GetProjFOVAngleY() override;
		/// @brief Returns the aspect ratio used by this editor camera.
		/// @return Current aspect ratio.
		const float			 GetAspectRatio() override;
		/// @brief Returns the units per pixel used by this editor camera.
		/// @return Current units per pixel.
		const float			 GetUnitsPerPixel() override;
		/// @brief Returns the near z used by this editor camera.
		/// @return Current near z.
		const float			 GetNearZ() override;
		/// @brief Returns the far z used by this editor camera.
		/// @return Current far z.
		const float			 GetFarZ() override;

		/// @brief Returns the off set used by this editor camera.
		/// @return Borrowed access to the off set.
		const Math::FTVector3&	GetOffSet() const override;
		/// @brief Returns the zoom factor used by this editor camera.
		/// @return Current zoom factor.
		const float				GetZoomFactor() const override;
		/// @brief Returns the resolution used by this editor camera.
		/// @return Current resolution.
		Math::FTVector2 GetResolution() const override;

		/// @brief Returns the debug rect used by this editor camera.
		/// @return Borrowed access to the debug rect.
		D3D11::FTRectangle* GetDebugRect();

		/// @brief Updates the position used by subsequent operations.
		/// @param pos Replacement position.
		void SetPosition(const Math::FTVector3& pos) override;
		/// @brief Updates the view type used by subsequent operations.
		/// @param viewType Replacement view type.
		void SetViewType(Core::Viewtype viewType) override;
		/// @brief Updates the target actor used by subsequent operations.
		/// @param actor Replacement target actor.
		void SetTargetActor(Core::IActor* actor) override;
		/// @brief Updates the offset used by subsequent operations.
		/// @param offset Replacement offset.
		void SetOffset(Math::FTVector3 offset) override;

		/// @brief Applies a multiplicative change to the editor camera zoom.
		/// @return Applies a multiplicative change to the editor camera zoom.
		float& ZoomFactor() override;

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param window Window used by the operation.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param unit Unit conversion factor.
		void		 Initialize(Core::IWindow* window, unsigned int pixels, float unit);
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice);
		/// @brief Advances frame-dependent state using the current time step.
		/// @param gameCamera Camera used to render the game scene.
		virtual void Update(Core::ICamera* gameCamera);

	public:
		/// @brief Initializes camera state used for editor navigation.
		EditorCamera();
		/// @brief Releases the resources managed by this instance during destruction.
		~EditorCamera() override;

	protected:
		/// @brief Establishes the conversion from engine world units to pixels.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param units World-space distance or unit count.
		void InitializePixelsPerUnit(unsigned int pixels, float units) override;

	private:
		D3D11::CameraData* mData;

		bool  mPanKeyPressed;
		float mPanValModSpeed;
		float mZoomValModSpeed;

		D3D11::FTRectangle* mDebugRect;

		ImVec2 mMiddleMouseClickedPos;
		ImVec2 mMiddleMouseClickedRot;

	private:
		/// @brief Moves the editor camera along its local horizontal and vertical axes.
		/// @param vec2 Two-dimensional vector read or written.
		void PanLocalXY(Math::FTVector2 vec2);
	};
} // namespace Editor
