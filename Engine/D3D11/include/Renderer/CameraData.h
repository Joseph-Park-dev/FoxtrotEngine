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

		D3D11_API Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos);
		D3D11_API Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const;
		D3D11_API Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const;

		D3D11_API void SaveProperties(std::ofstream& ofs);
		D3D11_API void LoadProperties(std::ifstream& ifs);

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		D3D11_API const Math::FTVector3& GetPosition() const;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		D3D11_API void GetViewMatrix(Math::FTMatrix4& outViewMat);
		D3D11_API void GetProjectionMatrix(Math::FTMatrix4& outProjMat);

		D3D11_API const Graphics::Viewtype GetViewType();
		D3D11_API const float				 GetProjFOVAngleY();
		D3D11_API const float				 GetAspectRatio();
		D3D11_API const float				 GetUnitsPerPixel();
		D3D11_API const float				 GetNearZ();
		D3D11_API const float				 GetFarZ();

		D3D11_API const Math::FTVector3&	GetOffSet() const;
		D3D11_API const float				GetZoomFactor() const;
		D3D11_API Math::FTVector2 GetResolution() const;

		D3D11_API void SetPosition(const Math::FTVector3& pos);
		D3D11_API void SetViewType(Graphics::Viewtype viewType);
		D3D11_API void SetTargetActor(Common::IActor* actor);
		D3D11_API void SetOffset(Math::FTVector3 offset);

		D3D11_API void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11