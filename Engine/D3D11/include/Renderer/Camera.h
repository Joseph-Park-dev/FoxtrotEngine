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
#include "Entity/Entity.h"

#include <iosfwd>

#include "Utility/SingletonMacro.h"
#include "FTMath.h"

namespace Core
{
	class FTWindow;
	class Actor;
	class SceneManager;
} // namespace Core

namespace D3D11
{
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

	enum class Viewtype
	{
		Orthographic,
		Perspective
	};

	class Camera :
		public Core::Entity
	{
		SINGLETON_PROTECTED(Camera)
	public:
		Math::FTVector3 ConvertScreenPosToWorld(Math::FTVector2 screenPos);
		Math::FTVector2 ConvertScreenPosToNDC(Math::FTVector2 screenPos);

		Math::FTVector3 ConvertToCenter(Math::FTVector3 topLeftPos, Math::FTVector2 renderSize);
		Math::FTVector3 ConvertToTopLeft(Math::FTVector3 centerPos, Math::FTVector2 renderSize);

		// void ConvertWorldPosToScreen(const Math::FTVector3& worldPos, const Transform* transform, Math::FTVector2& out);

	public:
		Math::FTMatrix4 GetViewRow();
		Math::FTMatrix4 GetProjRow();

		const Viewtype GetViewType();
		const float	   GetProjFOVAngleY();
		const float	   GetAspectRatio();
		const float	   GetPixelsPerUnit();
		const float	   GetNearZ();
		const float	   GetFarZ();

		const Math::FTVector3& GetPosition() const;
		const Math::FTVector2& GetResolution() const;
		const Math::FTVector3& GetOffSet() const;
		const float			   GetZoomFactor() const;

		void SetPosition(Math::FTVector3 pos);
		void SetPosition(float posX, float posY, float posZ);
		void SetViewType(Viewtype viewType);
		void SetTargetActor(Core::Actor* actor);
		void SetOffset(Math::FTVector3 offset);

	public:
		Math::FTVector3& Position();
		Math::FTVector3& Offset();
		float&			 ZoomFactor();

	protected:
		float& ZoomDelta();

	public:
		// "pixels" defines how much of them should fit in a given unit.
		virtual void Initialize(Core::FTWindow* renderWindow, unsigned int pixels, float unit);
		virtual void Update(float deltaTime);

	protected:
		virtual void Zoom();

	private:
		Core::FTWindow* mRenderWindow;
		Core::Actor*	mTarget;
		Math::FTVector3 mPosition;
		Math::FTVector3 mOffset;

		float mProjFOVAngleY;
		float mNearZ, mFarZ;
		float mAspect;
		float mPixelsPerUnit; // Used for pixel-perfect calculation.

		float mZoomFactor;
		float mZoomDelta;

		Viewtype mViewType;

	private:
		void InitializePixelsPerUnit(unsigned int pixels, float units = 1.f);

	public:
		void SaveProperties(std::ofstream& ofs);
		void LoadProperties(std::ifstream& ifs, Core::SceneManager* targetActor);
	};
} // namespace D3D11