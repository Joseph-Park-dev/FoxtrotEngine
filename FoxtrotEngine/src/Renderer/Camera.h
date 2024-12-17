#pragma once
#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#define CAMERA_TARGET_NONE	-1
#define HD_RESOLUTION		FTVector2(1920.f,1080.f)
#define LOOKAT_MODSPEED		0.01f
#define Drag_MODSPEED		0.001f

using Matrix = DirectX::SimpleMath::Matrix;
using Vector3 = DirectX::SimpleMath::Vector3;

enum class Viewtype {
	Orthographic,
	Perspective
};

class Camera
{
	SINGLETON_PROTECTED(Camera);

public:
	FTVector3 ConvertScreenPosToWorld(FTVector2 screenPos);

public:
	Matrix GetViewRow();
	Matrix GetProjRow();
	Vector3 GetEyePos();

	Viewtype GetViewType()		{ return mViewType; }
	float	 GetProjFOVAngleY() { return mProjFOVAngleY; }
	float	 GetAspectRatio()	{ return mAspect; }
	float	 GetPixelsPerUnit() { return mPixelsPerUnit; }

	void	 SetViewType(Viewtype viewType) { mViewType = viewType; }

public:
	// "pixels" define how much of them should fit in a given unit.
	void Initialize(FoxtrotRenderer* renderer, UINT pixels, float unit);
	virtual void Update(float deltaTime);

protected:
	void ZoomIn();

private:
	FoxtrotRenderer* mRenderer;
	
	Vector3 mPosition;
	Vector3 mViewDir;
	Vector3 mUpDir;
	Vector3 mRightDir;

	FTVector2 mDiff;
	FTVector2 mScreenCenter;

	// roll, pitch, yaw
	// https://en.wikipedia.org/wiki/Aircraft_principal_axes
	float mPitch, mYaw;
	float mProjFOVAngleY;
	float mNearZ, mFarZ;
	float mAspect;
	float mPixelsPerUnit;	// Used for pixel-perfect calculation.

	Viewtype mViewType;

private:
	void InitializePixelsPerUnit(float pixels, float units = 1.f);

#ifdef FOXTROT_EDITOR
private:
	ImVec2 mMiddleMouseClickedPos;
	ImVec2 mMiddleMouseClickedRot;

public:
	void DisplayCameraMenu();
#endif // FOXTROT_EDITOR
};