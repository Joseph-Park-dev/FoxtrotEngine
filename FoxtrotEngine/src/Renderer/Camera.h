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
#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#define CAMERA_TARGET_NONE -1
#define HD_RESOLUTION FTVector2(1920.f, 1080.f)
#define LOOKAT_MODSPEED 0.01f
#define Drag_MODSPEED 0.001f

using Matrix  = DirectX::SimpleMath::Matrix;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
class FTWindow;

namespace ChunkKey
{
	constexpr const char* CAMERA_DATA		  = "Camera Data";
	constexpr const char* TARGET_ACTOR		  = "Target Actor";
	constexpr const char* RENDER_RESOLUTION	  = "Render Resolution";
	constexpr const char* RENDER_SCREENCENTER = "Screen Center";
	constexpr const char* CAM_POSITION		  = "Position";
	constexpr const char* CAM_OFFSET		  = "Offset";
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

public:
	FTWindow* GetRenderWindow() const;
	Matrix	  GetViewRow();
	Matrix	  GetProjRow();

	Viewtype GetViewType();
	float	 GetProjFOVAngleY();
	float	 GetAspectRatio();
	float	 GetPixelsPerUnit();
	float	 GetNearZ();
	float	 GetFarZ();

	const FTVector3& GetPosition() const;
	FTVector2&		 GetResolution() const;
	const FTVector3& GetOffSet() const;

	void SetPosition(FTVector3 pos);
	void SetViewType(Viewtype viewType);
	void SetTargetActor(Actor* actor);
	void SetOffset(FTVector3 offset);

protected:
	Vector3& Position();
	Vector3& FrontDir();
	Vector3& UpDir();
	Vector3& RightDir();

	float& ZoomDelta();

public:
	// "pixels" defines how much of them should fit in a given unit.
	virtual void Initialize(FTWindow* renderWindow, UINT pixels, float unit);
	virtual void Update(float deltaTime);

	virtual void UpdateViewDirections();

protected:
	virtual void Zoom();

private:
	FTWindow* mRenderWindow; // The scene will be rendered on this window.
	Actor*	  mTarget;

	Vector3 mPosition;
	Vector3 mFrontDir;
	Vector3 mUpDir;
	Vector3 mRightDir;

	FTVector2 mResolution;
	FTVector2 mDiff;
	FTVector2 mScreenCenter;
	FTVector3 mOffset;

	// roll, pitch, yaw
	// https://en.wikipedia.org/wiki/Aircraft_principal_axes
	float mPitch, mYaw;
	float mProjFOVAngleY;
	float mNearZ, mFarZ;
	float mAspect;
	float mPixelsPerUnit; // Used for pixel-perfect calculation.

	float mZoomFactor;
	float mZoomDelta;

	Viewtype mViewType;

private:
	void InitializePixelsPerUnit(UINT pixels, float units = 1.f);

public:
	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
private:
	ImVec2 mMiddleMouseClickedPos;
	ImVec2 mMiddleMouseClickedRot;
#endif // FOXTROT_EDITOR
};