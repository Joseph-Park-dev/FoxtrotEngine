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
#include "Renderer/ICamera.h"

#include <iosfwd>

#include "Utility/SingletonMacro.h"
#include "FTMath.h"

namespace Core
{
	class IActor;
	class SceneManager;
	struct CameraData;
} // namespace Core

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

		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) override;
		virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const override;
		virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	public:
		virtual Core::CameraData* Data() override;

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		virtual const Math::FTVector3& GetPosition() const override;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) override;
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) override;

	public:
		const Core::Viewtype GetViewType() override;
		const float			 GetProjFOVAngleY() override;
		const float			 GetAspectRatio() override;
		const float			 GetUnitsPerPixel() override;
		const float			 GetNearZ() override;
		const float			 GetFarZ() override;

		const Math::FTVector3&	GetOffSet() const override;
		const float				GetZoomFactor() const override;
		const Math::FTVector2&& GetResolution() const override;

		void SetPosition(const Math::FTVector3& pos) override;
		void SetViewType(Core::Viewtype viewType) override;
		void SetTargetActor(Core::IActor* actor) override;
		void SetOffset(Math::FTVector3 offset) override;

	public:
		Math::FTVector3& Position();
		Math::FTVector3& Offset();
		float&			 ZoomFactor() override;

	public:
		Camera(Core::CameraData* data);
		~Camera() override;

	public:
		// "pixels" defines how much of them should fit in a given unit.
		virtual void Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit);
		//	virtual void Update(float deltaTime);

		// protected:
		//	virtual void Zoom();

	protected:
		float& ZoomDelta();

	private:
		Core::CameraData* mData;

	private:
		void InitializeUnitsPerPixel(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11