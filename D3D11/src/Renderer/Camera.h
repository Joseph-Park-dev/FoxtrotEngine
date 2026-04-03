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
#include <iosfwd>

#include "SingletonMacro.h"
#include "Math/FTMath.h"

class FTWindow;
class Actor;
class SceneManager;

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

	class Camera
	{
		SINGLETON_PROTECTED(Camera)

	public:
		FTVector3 ConvertScreenPosToWorld(FTVector2 screenPos);
		FTVector2 ConvertScreenPosToNDC(FTVector2 screenPos);

		FTVector3 ConvertToCenter(FTVector3 topLeftPos, FTVector2 renderSize);
		FTVector3 ConvertToTopLeft(FTVector3 centerPos, FTVector2 renderSize);

		// void ConvertWorldPosToScreen(const FTVector3& worldPos, const Transform* transform, FTVector2& out);

	public:
		FTMatrix4 GetViewRow();
		FTMatrix4 GetProjRow();

		const Viewtype GetViewType();
		const float	   GetProjFOVAngleY();
		const float	   GetAspectRatio();
		const float	   GetPixelsPerUnit();
		const float	   GetNearZ();
		const float	   GetFarZ();

		const FTVector3& GetPosition() const;
		const FTVector2& GetResolution() const;
		const FTVector3& GetOffSet() const;
		const float		 GetZoomFactor() const;

		void SetPosition(FTVector3 pos);
		void SetPosition(float posX, float posY, float posZ);
		void SetViewType(Viewtype viewType);
		void SetTargetActor(Actor* actor);
		void SetOffset(FTVector3 offset);

	public:
		FTVector3& Offset();

	protected:
		float& ZoomDelta();

	public:
		// "pixels" defines how much of them should fit in a given unit.
		virtual void Initialize(FTWindow* renderWindow, unsigned int pixels, float unit);
		virtual void Update(float deltaTime);

	protected:
		virtual void Zoom();

	private:
		FTWindow* mRenderWindow;
		Actor*	  mTarget;
		FTVector3 mPosition;
		FTVector3 mOffset;

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
		void LoadProperties(std::ifstream& ifs, SceneManager* targetActor);

#ifdef FOXTROT_EDITOR
	public:
		float& ZoomFactor();

	private:
		ImVec2 mMiddleMouseClickedPos;
		ImVec2 mMiddleMouseClickedRot;
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11