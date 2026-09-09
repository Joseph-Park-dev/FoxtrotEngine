// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Camera class which decides how the scene will be rendered.
/// Think of this as a window to the virtual game world.
/// </summary>

#pragma once
#include "Plugin/D3D11Exports.h"
#include "Renderer/ICamera.h"

#include <iosfwd>

#include "Utility/SingletonMacro.h"
#include "FTMath.h"

namespace Core
{
	class SceneManager;
} // namespace Core

namespace D3D11
{
	struct CameraData;
} // namespace D3D11

namespace D3D11
{
	class D3D11Window;
	class Camera :
		public Core::ICamera
	{
	public:
		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		/// @brief Converts a screen-space position into world coordinates using the camera transforms.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts a screen-space position into world coordinates using the camera transforms.
		D3D11_API virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) override;
		/// @brief Projects a world-space position into screen coordinates.
		/// @param worldPos Position in world coordinates.
		/// @return Current world to screen.
		D3D11_API virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const override;
		/// @brief Converts screen coordinates to normalized device coordinates.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts screen coordinates to normalized device coordinates.
		D3D11_API virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const override;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		D3D11_API void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		D3D11_API void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Exposes the stored payload for consumers of this resource or container.
		/// @return Borrowed access to the data.
		D3D11_API virtual D3D11::CameraData* Data();

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		/// @brief Returns the position used by this camera.
		/// @return Borrowed access to the position.
		D3D11_API virtual const Math::FTVector3& GetPosition() const override;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// @brief Returns the view matrix used by this camera.
		/// @param outViewMat Receives the view matrix.
		D3D11_API virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) override;
		/// @brief Returns the projection matrix used by this camera.
		/// @param outProjMat Receives the projection matrix.
		D3D11_API virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) override;

	public:
		/// @brief Returns the view type used by this camera.
		/// @return Current view type.
		D3D11_API const Core::Viewtype GetViewType() override;
		/// @brief Returns the proj fovangle y used by this camera.
		/// @return Current proj fovangle y.
		D3D11_API const float			 GetProjFOVAngleY() override;
		/// @brief Returns the aspect ratio used by this camera.
		/// @return Current aspect ratio.
		D3D11_API const float			 GetAspectRatio() override;
		/// @brief Returns the units per pixel used by this camera.
		/// @return Current units per pixel.
		D3D11_API const float			 GetUnitsPerPixel() override;
		/// @brief Returns the near z used by this camera.
		/// @return Current near z.
		D3D11_API const float			 GetNearZ() override;
		/// @brief Returns the far z used by this camera.
		/// @return Current far z.
		D3D11_API const float			 GetFarZ() override;

		/// @brief Returns the off set used by this camera.
		/// @return Borrowed access to the off set.
		D3D11_API const Math::FTVector3&	GetOffSet() const override;
		/// @brief Returns the zoom factor used by this camera.
		/// @return Current zoom factor.
		D3D11_API const float				GetZoomFactor() const override;
		/// @brief Returns the resolution used by this camera.
		/// @return Current resolution.
		D3D11_API Math::FTVector2 GetResolution() const override;

		/// @brief Updates the position used by subsequent operations.
		/// @param pos Replacement position.
		D3D11_API void SetPosition(const Math::FTVector3& pos) override;
		/// @brief Updates the view type used by subsequent operations.
		/// @param viewType Replacement view type.
		D3D11_API void SetViewType(Core::Viewtype viewType) override;
		/// @brief Updates the target actor used by subsequent operations.
		/// @param actor Replacement target actor.
		D3D11_API void SetTargetActor(Core::IActor* actor) override;
		/// @brief Updates the offset used by subsequent operations.
		/// @param offset Replacement offset.
		D3D11_API void SetOffset(Math::FTVector3 offset) override;

	public:
		/// @brief Exposes the stored spatial position.
		/// @return Borrowed access to the position.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Math::FTVector3& Position();
		/// @brief Exposes the drawing offset applied to the stored geometry.
		/// @return Borrowed access to the offset.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Math::FTVector3& Offset();
		/// @brief Applies a multiplicative change to the editor camera zoom.
		/// @return Applies a multiplicative change to the editor camera zoom.
		D3D11_API float&			 ZoomFactor() override;

	public:
		/// @brief Initializes camera pose, projection settings, and rendering constants.
		D3D11_API Camera();
		/// @brief Releases the resources managed by this instance during destruction.
		D3D11_API ~Camera() override;

	public:
		// "pixels" defines how much of them should fit in a given unit.
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderWindow Window associated with the render target.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param unit Unit conversion factor.
		D3D11_API virtual void Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit);
		//	virtual void Update(float deltaTime);

		// protected:
		//	virtual void Zoom();

	protected:
		/// @brief Establishes the conversion from engine world units to pixels.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param units World-space distance or unit count.
		D3D11_API virtual void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f) override;
		/// @brief Adjusts the editor camera zoom by a relative amount.
		/// @return Adjusts the editor camera zoom by a relative amount.
		D3D11_API float& ZoomDelta();

	private:
		D3D11::CameraData* mData;

	private:
		/// @brief Establishes the conversion from pixels to engine world units.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param units World-space distance or unit count.
		D3D11_API void InitializeUnitsPerPixel(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11
