#pragma once
#include "Plugin/D3D11Exports.h"
#include <iosfwd>

#include "Renderer/ICamera.h"
#include "FTMath.h"

namespace Common
{
	class IActor;
} // namespace Common

namespace D3D11
{
	struct CameraData
	{
		Common::IActor*	   Target	= nullptr;
		Graphics::Viewtype ViewType = Graphics::Viewtype::Orthographic;

		/// @brief Screen width (Resolution X)
		unsigned int ResX = 1920;
		/// @brief Screen Height (Resolution Y)
		unsigned int ResY = 1080;

		Math::FTVector3 Position = Math::FTVector3(0.0f, 0.0f, -5.0f);
		Math::FTVector3 Offset	 = Math::FTVector3::Zero;

		float ProjFOVAngleY = 70.f;
		float NearZ			= 0.01f;
		float FarZ			= 100.0f;
		float Aspect		= 0.0f;
		float UnitsPerPixel = 0.0f; // Used for pixel-perfect calculation.

		float ZoomFactor = 1.0f;
		float ZoomDelta	 = 0.0f;

		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		/// @brief Converts a screen-space position into world coordinates using the camera transforms.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts a screen-space position into world coordinates using the camera transforms.
		D3D11_API Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos);
		/// @brief Projects a world-space position into screen coordinates.
		/// @param worldPos Position in world coordinates.
		/// @return Current world to screen.
		D3D11_API Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const;
		/// @brief Converts screen coordinates to normalized device coordinates.
		/// @param screenPos Position in screen coordinates.
		/// @return Converts screen coordinates to normalized device coordinates.
		D3D11_API Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const;

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		D3D11_API void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		D3D11_API void LoadProperties(std::ifstream& ifs);

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		/// @brief Returns the position used by this camera data.
		/// @return Borrowed access to the position.
		D3D11_API const Math::FTVector3& GetPosition() const;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// @brief Returns the view matrix used by this camera data.
		/// @param outViewMat Receives the view matrix.
		D3D11_API void GetViewMatrix(Math::FTMatrix4& outViewMat);
		/// @brief Returns the projection matrix used by this camera data.
		/// @param outProjMat Receives the projection matrix.
		D3D11_API void GetProjectionMatrix(Math::FTMatrix4& outProjMat);

		/// @brief Returns the view type used by this camera data.
		/// @return Current view type.
		D3D11_API const Graphics::Viewtype GetViewType();
		/// @brief Returns the proj fovangle y used by this camera data.
		/// @return Current proj fovangle y.
		D3D11_API const float				 GetProjFOVAngleY();
		/// @brief Returns the aspect ratio used by this camera data.
		/// @return Current aspect ratio.
		D3D11_API const float				 GetAspectRatio();
		/// @brief Returns the units per pixel used by this camera data.
		/// @return Current units per pixel.
		D3D11_API const float				 GetUnitsPerPixel();
		/// @brief Returns the near z used by this camera data.
		/// @return Current near z.
		D3D11_API const float				 GetNearZ();
		/// @brief Returns the far z used by this camera data.
		/// @return Current far z.
		D3D11_API const float				 GetFarZ();

		/// @brief Returns the off set used by this camera data.
		/// @return Borrowed access to the off set.
		D3D11_API const Math::FTVector3&	GetOffSet() const;
		/// @brief Returns the zoom factor used by this camera data.
		/// @return Current zoom factor.
		D3D11_API const float				GetZoomFactor() const;
		/// @brief Returns the resolution used by this camera data.
		/// @return Current resolution.
		D3D11_API Math::FTVector2 GetResolution() const;

		/// @brief Updates the position used by subsequent operations.
		/// @param pos Replacement position.
		D3D11_API void SetPosition(const Math::FTVector3& pos);
		/// @brief Updates the view type used by subsequent operations.
		/// @param viewType Replacement view type.
		D3D11_API void SetViewType(Graphics::Viewtype viewType);
		/// @brief Updates the target actor used by subsequent operations.
		/// @param actor Replacement target actor.
		D3D11_API void SetTargetActor(Common::IActor* actor);
		/// @brief Updates the offset used by subsequent operations.
		/// @param offset Replacement offset.
		D3D11_API void SetOffset(Math::FTVector3 offset);

		/// @brief Establishes the conversion from engine world units to pixels.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param units World-space distance or unit count.
		D3D11_API void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11
