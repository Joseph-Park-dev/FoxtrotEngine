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

		D3D11_API virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) override;
		D3D11_API virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const override;
		D3D11_API virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const override;

	public:
		D3D11_API void SaveProperties(std::ofstream& ofs) override;
		D3D11_API void LoadProperties(std::ifstream& ifs) override;

	public:
		D3D11_API virtual D3D11::CameraData* Data();

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		D3D11_API virtual const Math::FTVector3& GetPosition() const override;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		D3D11_API virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) override;
		D3D11_API virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) override;

	public:
		D3D11_API const Core::Viewtype GetViewType() override;
		D3D11_API const float			 GetProjFOVAngleY() override;
		D3D11_API const float			 GetAspectRatio() override;
		D3D11_API const float			 GetUnitsPerPixel() override;
		D3D11_API const float			 GetNearZ() override;
		D3D11_API const float			 GetFarZ() override;

		D3D11_API const Math::FTVector3&	GetOffSet() const override;
		D3D11_API const float				GetZoomFactor() const override;
		D3D11_API Math::FTVector2 GetResolution() const override;

		D3D11_API void SetPosition(const Math::FTVector3& pos) override;
		D3D11_API void SetViewType(Core::Viewtype viewType) override;
		D3D11_API void SetTargetActor(Core::IActor* actor) override;
		D3D11_API void SetOffset(Math::FTVector3 offset) override;

	public:
		D3D11_API Math::FTVector3& Position();
		D3D11_API Math::FTVector3& Offset();
		D3D11_API float&			 ZoomFactor() override;

	public:
		D3D11_API Camera();
		D3D11_API ~Camera() override;

	public:
		// "pixels" defines how much of them should fit in a given unit.
		D3D11_API virtual void Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit);
		//	virtual void Update(float deltaTime);

		// protected:
		//	virtual void Zoom();

	protected:
		D3D11_API virtual void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f) override;
		D3D11_API float& ZoomDelta();

	private:
		D3D11::CameraData* mData;

	private:
		D3D11_API void InitializeUnitsPerPixel(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11