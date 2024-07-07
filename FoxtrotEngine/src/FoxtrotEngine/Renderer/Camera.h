#pragma once
#include "FoxtrotEngine/Core/SingletonMacro.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Actors/Actor.h"

#define CAMERA_TARGET_NONE	-1
#define HD_RESOLUTION		FTVector2(1920.f,1080.f)
#define LOOKAT_MODSPEED		0.01f
#define Drag_MODSPEED		0.001f

class Camera
{
	SINGLETON_PROTECTED(Camera);

public:
	FTVector3			GetViewEyePos()			{ return mViewEyePos; }
	FTVector3			GetViewEyeDir()			{ return mViewEyeDir; }
	FTVector3			GetViewUp()				{ return mViewUp; }

	DirectX::XMFLOAT3	GetViewEyePosDX()		{ return mViewEyePos.GetDXVec3(); }
	DirectX::XMFLOAT3	GetViewEyeDirDX()		{ return mViewEyeDir.GetDXVec3(); }
	DirectX::XMFLOAT3	GetViewUpDX()			{ return mViewUp.GetDXVec3(); }

	float				GetProjFOVAngleY()		{ return mProjFOVAngleY; }
	float				GetAspect()				{ return mAspect; }
	float				GetNearZ()				{ return mNearZ; }
	float				GetFarZ()				{ return mFarZ; }

	FTVector3			GetViewEyePosition()	{ return mViewEyePosition; }
	FTVector3			GetViewEyeRotation()	{ return mViewEyeRotation; }
	
	int					GetTargetActorID()		{ return mTargetActorID; }
	FTVector2			GetRenderResolution()	{ return mRenderResolution; }

	void	SetViewEyePos		(FTVector3 pos) { mViewEyePos = pos; }
	void	SetRenderResolution	(FTVector2 res) { mRenderResolution = res; }

private:
	FTVector3	mViewEyePos;
	FTVector3	mViewEyeDir;
	FTVector3	mViewUp;
	FTVector2	mRenderResolution;
	float		mProjFOVAngleY;
	float		mAspect;
	float		mNearZ;
	float		mFarZ;

	// Camrea Object Transformation
	FTVector3	mViewEyePosition; // Additional rotation given to camera
	FTVector3	mViewEyeRotation; // Additional rotation given to camera

	Actor*		mTargetActor;
	int			mTargetActorID;

public:
	void Initialize();
	virtual void Update(float deltaTime);

#ifdef _DEBUG
	// void Render(FoxtrotRenderer* renderer);
	void DisplayCameraMenu();

private:
	ImVec2 mMiddleMouseClickedPos;
	ImVec2 mMiddleMouseClickedRot;
#endif // _DEBUG

protected:
	void ZoomIn();
	void ZoomOut();

private:
	float	GetAspectRatio() const { return (float)mRenderResolution.x / (float)mRenderResolution.y; }
	void	UpdateTargetActor();
	void	SetTarget(int id);
};