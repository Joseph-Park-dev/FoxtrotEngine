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
	Matrix GetViewRow();
	Matrix GetProjRow();
	Vector3 GetEyePos();

	Viewtype GetViewType() { return mViewType; }
	float	 GetProjFOVAngleY() { return mProjFOVAngleY; }
	float	 GetAspectRatio() { return mAspect; }

private:
	Vector3 mPosition	= Vector3(0.0f, 0.4f, 0.0f);
	Vector3 mViewDir	= Vector3(0.0f, 0.0f, 1.0f);
	Vector3 mUpDir		= Vector3(0.0f, 1.0f, 0.0f); // �̹� ���������� ����
	Vector3 mRightDir	= Vector3(1.0f, 0.0f, 0.0f);

	// roll, pitch, yaw
	// https://en.wikipedia.org/wiki/Aircraft_principal_axes
	float mPitch = 0.0f, mYaw = 0.0f;
	float mSpeed = 1.0f; // �����̴� �ӵ�

	// �������� �ɼǵ� ī�޶� Ŭ������ �̵�
	float mProjFOVAngleY = 70.0f;
	float mNearZ = 0.01f;
	float mFarZ = 100.0f;
	float mAspect = 1920.0f / 1080.0f;

	Viewtype mViewType;

public:
	void Initialize();
	virtual void Update(float deltaTime);

private:
	ImVec2 mMiddleMouseClickedPos;
	ImVec2 mMiddleMouseClickedRot;
 // _DEBUG

protected:
	void ZoomIn();

#ifdef FOXTROT_EDITOR
public:
	void DisplayCameraMenu();
#endif // FOXTROT_EDITOR
};