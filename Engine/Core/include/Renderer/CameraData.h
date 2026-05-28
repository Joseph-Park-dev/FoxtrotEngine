#pragma once
#include "Renderer/ICamera.h"

#include "Actor/IActor.h"

namespace Core
{
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
} // namespace Core