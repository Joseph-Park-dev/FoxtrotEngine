#pragma once
#include <iosfwd>
#include "ICamera.h"
#include "FTMath.h"

namespace Core
{
	class IActor;

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

		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos);
		Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const;
		Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const;

		void SaveProperties(std::ofstream& ofs);
		void LoadProperties(std::ifstream& ifs);

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		const Math::FTVector3& GetPosition() const;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		void GetViewMatrix(Math::FTMatrix4& outViewMat);
		void GetProjectionMatrix(Math::FTMatrix4& outProjMat);

		const Core::Viewtype GetViewType();
		const float			 GetProjFOVAngleY();
		const float			 GetAspectRatio();
		const float			 GetUnitsPerPixel();
		const float			 GetNearZ();
		const float			 GetFarZ();

		const Math::FTVector3&	GetOffSet() const;
		const float				GetZoomFactor() const;
		const Math::FTVector2&& GetResolution() const;

		void SetPosition(const Math::FTVector3& pos);
		void SetViewType(Core::Viewtype viewType);
		void SetTargetActor(Core::IActor* actor);
		void SetOffset(Math::FTVector3 offset);

		void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f);
	};
} // namespace Core