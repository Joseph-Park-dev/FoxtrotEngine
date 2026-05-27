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
} // namespace Core

namespace D3D11
{
	class D3D11Window;

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

	class Camera :
		public Core::ICamera
	{
		SINGLETON(Camera)

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
		const D3D11::D3D11Window* GetRenderWindow();
		const Core::Viewtype	  GetViewType() override;
		const float				  GetProjFOVAngleY() override;
		const float				  GetAspectRatio() override;
		const float				  GetPixelsPerUnit() override;
		const float				  GetNearZ() override;
		const float				  GetFarZ() override;

		const Math::FTVector3& GetOffSet() const override;
		const float			   GetZoomFactor() const override;

		void SetPosition(const Math::FTVector3& pos) override;
		void SetViewType(Core::Viewtype viewType) override;
		void SetTargetActor(Core::IActor* actor) override;
		void SetOffset(Math::FTVector3 offset) override;

	public:
		Math::FTVector3& Position();
		Math::FTVector3& Offset();
		float&			 ZoomFactor();

	protected:
		float& ZoomDelta();

	public:
		// "pixels" defines how much of them should fit in a given unit.
		virtual void Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit);
		//	virtual void Update(float deltaTime);

		// protected:
		//	virtual void Zoom();

	private:
		D3D11::D3D11Window*		mRenderWindow;
		Core::IActor*			mTarget;
		mutable Math::FTVector3 mPosition;
		Math::FTVector3			mOffset;

		float mProjFOVAngleY;
		float mNearZ, mFarZ;
		float mAspect;
		float mPixelsPerUnit; // Used for pixel-perfect calculation.

		float mZoomFactor;
		float mZoomDelta;

		Core::Viewtype mViewType;

	private:
		void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f);
	};
} // namespace D3D11