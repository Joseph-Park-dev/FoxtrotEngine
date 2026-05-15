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
#include "FTMath.h"

namespace Core
{
	/// <summary>
	/// Abstract camera interface for view/projection transformations.
	/// </summary>
	class ICamera
	{
	public:
		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		/// <summary>
		/// Converts screen coordinates to world coordinates.
		/// </summary>
		/// <param name="screenPos">Position in screen pixels.</param>
		/// <returns>Position in world space.</returns>
		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) const = 0;

		/// <summary>
		/// Converts world coordinates to screen coordinates.
		/// </summary>
		/// <param name="worldPos">Position in world space.</param>
		/// <returns>Position in screen pixels.</returns>
		virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const = 0;

		/// <summary>
		/// Converts screen coordinates to normalized device coordinates.
		/// </summary>
		/// <param name="screenPos">Position in screen pixels.</param>
		/// <returns>Position in NDC (-1 to 1 range).</returns>
		virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const = 0;

	public:
		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the camera position in world space.
		/// </summary>
		virtual const Math::FTVector3& GetPosition() const = 0;

		/// <summary>
		/// Gets the camera rotation (pitch, yaw, roll in radians).
		/// </summary>
		virtual const Math::FTVector3& GetRotation() const = 0;

		/// <summary>
		/// Sets the camera position in world space.
		/// </summary>
		virtual void SetPosition(const Math::FTVector3& position) = 0;

		/// <summary>
		/// Sets the camera rotation (pitch, yaw, roll in radians).
		/// </summary>
		virtual void SetRotation(const Math::FTVector3& rotation) = 0;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the view matrix (world-to-camera transformation).
		/// </summary>
		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) const = 0;

		/// <summary>
		/// Gets the projection matrix (camera-to-clip transformation).
		/// </summary>
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) const = 0;

		//////////////////////////////////////////
		////// Projection Settings ///////////////
		//////////////////////////////////////////

		/// <summary>
		/// Sets orthographic projection parameters for 2D rendering.
		/// </summary>
		/// <param name="width">View width in world units.</param>
		/// <param name="height">View height in world units.</param>
		/// <param name="nearPlane">Near clipping plane distance.</param>
		/// <param name="farPlane">Far clipping plane distance.</param>
		virtual void SetOrthographic(float width, float height, float nearPlane, float farPlane) = 0;

		/// <summary>
		/// Sets perspective projection parameters for 3D rendering.
		/// </summary>
		/// <param name="fovY">Vertical field of view in radians.</param>
		/// <param name="aspectRatio">Width/height aspect ratio.</param>
		/// <param name="nearPlane">Near clipping plane distance.</param>
		/// <param name="farPlane">Far clipping plane distance.</param>
		virtual void SetPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane) = 0;

	public:
		virtual ~ICamera() = default;
	};
} // namespace Core
