// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pure virtual interface for camera implementations.
/// Provides view and projection matrix abstractions.
/// </summary>

#pragma once
#include <iosfwd>

#include "FTMath.h"

namespace Common
{
	class IActor;
} // namespace Common

namespace Graphics
{
	enum class Viewtype
	{
		Orthographic,
		Perspective
	};

	/// <summary>
	/// Abstract camera interface for view/projection transformations.
	/// </summary>
	class ICamera
	{
	public:
		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		/// @brief Converts a screen-space position into world coordinates using the camera transforms.
		/// Converts screen coordinates to world coordinates.
		/// <param name="screenPos">Position in screen pixels.</param>
		/// @param screenPos Position in screen coordinates.
		/// @return Converts a screen-space position into world coordinates using the camera transforms.
		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) = 0;

		/// @brief Projects a world-space position into screen coordinates.
		/// Converts world coordinates to screen coordinates.
		/// <param name="worldPos">Position in world space.</param>
		/// @param worldPos Position in world coordinates.
		/// @return Current world to screen.
		virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const = 0;

		/// @brief Converts screen coordinates to normalized device coordinates.
		/// Converts screen coordinates to normalized device coordinates.
		/// <param name="screenPos">Position in screen pixels.</param>
		/// @param screenPos Position in screen coordinates.
		/// @return Converts screen coordinates to normalized device coordinates.
		virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const = 0;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;

	public:
		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		/// @brief Returns the position used by this icamera.
		/// Gets the camera position in world space.
		/// @return Borrowed access to the position.
		virtual const Math::FTVector3& GetPosition() const = 0;

		/// @brief Returns the view type used by this icamera.
		/// @return Current view type.
		virtual const Viewtype GetViewType()	  = 0;
		/// @brief Returns the proj fovangle y used by this icamera.
		/// @return Current proj fovangle y.
		virtual const float	   GetProjFOVAngleY() = 0;
		/// @brief Returns the aspect ratio used by this icamera.
		/// @return Current aspect ratio.
		virtual const float	   GetAspectRatio()	  = 0;
		/// @brief Returns the units per pixel used by this icamera.
		/// @return Current units per pixel.
		virtual const float	   GetUnitsPerPixel() = 0;
		/// @brief Returns the near z used by this icamera.
		/// @return Current near z.
		virtual const float	   GetNearZ()		  = 0;
		/// @brief Returns the far z used by this icamera.
		/// @return Current far z.
		virtual const float	   GetFarZ()		  = 0;

		/// @brief Returns the off set used by this icamera.
		/// @return Borrowed access to the off set.
		virtual const Math::FTVector3&	GetOffSet() const	  = 0;
		/// @brief Returns the zoom factor used by this icamera.
		/// @return Current zoom factor.
		virtual const float				GetZoomFactor() const = 0;
		/// @brief Returns the resolution used by this icamera.
		/// @return Current resolution.
		virtual Math::FTVector2 GetResolution() const = 0;

		/// @brief Updates the position used by subsequent operations.
		/// Sets the camera position in world space.
		/// @param position Replacement position.
		virtual void SetPosition(const Math::FTVector3& position) = 0;
		/// @brief Updates the view type used by subsequent operations.
		/// @param viewType Replacement view type.
		virtual void SetViewType(Graphics::Viewtype viewType)	  = 0;
		/// @brief Updates the target actor used by subsequent operations.
		/// @param actor Replacement target actor.
		virtual void SetTargetActor(Common::IActor* actor)		  = 0;
		/// @brief Updates the offset used by subsequent operations.
		/// @param offset Replacement offset.
		virtual void SetOffset(Math::FTVector3 offset)			  = 0;

		/// @brief Applies a multiplicative change to the editor camera zoom.
		/// @return Applies a multiplicative change to the editor camera zoom.
		virtual float& ZoomFactor() = 0;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// @brief Returns the view matrix used by this icamera.
		/// Gets the view matrix (world-to-camera transformation).
		/// @param outViewMat Receives the view matrix.
		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) = 0;

		/// @brief Returns the projection matrix used by this icamera.
		/// Gets the projection matrix (camera-to-clip transformation).
		/// @param outProjMat Receives the projection matrix.
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~ICamera() = default;

	protected:
		/// @brief Establishes the conversion from engine world units to pixels.
		/// @param pixels Pixel data or pixel dimensions.
		/// @param units World-space distance or unit count.
		virtual void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f) = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* CAMERA_DATA		  = "Camera Data";
		constexpr const char* TARGET_ACTOR		  = "Target Actor";
		constexpr const char* RENDER_RESOLUTION	  = "Render Resolution";
		constexpr const char* RENDER_SCREENCENTER = "Screen Center";
		constexpr const char* CAM_POSITION		  = "Position";
		constexpr const char* CAM_OFFSET		  = "Offset";
		constexpr const char* CAM_ZOOM			  = "Zoom";
	} // namespace ChunkKey
} // namespace Graphics
