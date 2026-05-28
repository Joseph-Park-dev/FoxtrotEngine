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

#include <iosfwd>

namespace Core
{
	enum class Viewtype
	{
		Orthographic,
		Perspective
	};

	class IActor;
	class SceneManager;

	struct CameraData
	{
		Core::IActor*  Target	= nullptr;
		Core::Viewtype ViewType = Core::Viewtype::Orthographic;

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

		const Math::FTVector2&& GetResolution() const
		{
			return Math::FTVector2(ResX, ResY);
		}
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

		/// <summary>
		/// Converts screen coordinates to world coordinates.
		/// </summary>
		/// <param name="screenPos">Position in screen pixels.</param>
		/// <returns>Position in world space.</returns>
		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) = 0;

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
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void LoadProperties(std::ifstream& ifs) = 0;

	public:
		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		virtual CameraData* Data() = 0;

		/// <summary>
		/// Gets the camera position in world space.
		/// </summary>
		virtual const Math::FTVector3& GetPosition() const = 0;

		virtual const Viewtype GetViewType()	  = 0;
		virtual const float	   GetProjFOVAngleY() = 0;
		virtual const float	   GetAspectRatio()	  = 0;
		virtual const float	   GetUnitsPerPixel() = 0;
		virtual const float	   GetNearZ()		  = 0;
		virtual const float	   GetFarZ()		  = 0;

		virtual const Math::FTVector3&	GetOffSet() const	  = 0;
		virtual const float				GetZoomFactor() const = 0;
		virtual const Math::FTVector2&& GetResolution() const = 0;

		/// <summary>
		/// Sets the camera position in world space.
		/// </summary>
		virtual void SetPosition(const Math::FTVector3& position) = 0;
		virtual void SetViewType(Core::Viewtype viewType)		  = 0;
		virtual void SetTargetActor(Core::IActor* actor)		  = 0;
		virtual void SetOffset(Math::FTVector3 offset)			  = 0;

		virtual float& ZoomFactor() = 0;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the view matrix (world-to-camera transformation).
		/// </summary>
		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) = 0;

		/// <summary>
		/// Gets the projection matrix (camera-to-clip transformation).
		/// </summary>
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) = 0;

	public:
		virtual ~ICamera() = default;

	protected:
		virtual void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f) = 0;
	};
} // namespace Core
