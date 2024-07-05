#include "FoxtrotEngine/Renderer/Camera.h"

#include "FoxtrotEngine/Core/FTCore.h"

void Camera::Initialize()
{
}

void Camera::Update(float deltaTime)
{
}

Camera::Camera()
	: mViewEyePos	 (0.0f, 0.0f, -10.0f)
	, mViewEyeDir	 (0.0f, 0.0f, 1.0f)
	, mViewUp		 (0.0f, 1.0f, 0.0f)
	, mProjFOVAngleY (70.0f)
	, mNearZ		 (0.01f)
	, mFarZ			 (100.0f)
	, mRenderResolution(HD_RESOLUTION)
	, mAspect		 (GetAspectRatio())
	, mTargetActor	 (nullptr)
	, mTargetActorID (CAMERA_TARGET_NONE)
{}

Camera::~Camera(){}
