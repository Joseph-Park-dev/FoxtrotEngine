// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/Camera.h"

#include "directxtk/SimpleMath.h"

#include "WindowSystem/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "Core/FTCore.h"
#include "InputSystem/FTInputDevice.h"
#include "Managers/SceneManager.h"
#include "Scenes/Scene.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "Actors/Transform.h"

#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

void Camera::Initialize(FTWindow* renderWindow, UINT pixels, float unit)
{
	assert(!mRenderWindow); // Overlapped assignment must be prevented.

	mRenderWindow = renderWindow;
	InitializePixelsPerUnit(pixels, unit);
}

void Camera::Update(float deltaTime)
{
	Zoom();
}

void Camera::UpdateViewDirections()
{
	float yaw	= Math::ToRadians(mYaw);
	float pitch = Math::ToRadians(mPitch);

	Matrix rotMatrix = Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f).Transpose();
	mUpDir			 = Vector3::Transform(mUpDir, rotMatrix);
	mFrontDir		 = Vector3::Transform(mFrontDir, rotMatrix);

	mUpDir.Normalize();
	mFrontDir.Normalize();

	LogVector3(mFrontDir);

	mRightDir = mUpDir.Cross(mFrontDir);

	mRightDir.Normalize();
}

void Camera::Zoom()
{
	Math::Clampf(mZoomFactor, 0.01f, 3.0f);
	mZoomFactor += mZoomDelta;
	LogFloat(mZoomFactor);
}

Camera::Camera()
	: mRenderWindow(nullptr)
	, mTarget(nullptr)
	, mPosition(Vector3(0.0f, 0.0f, -5.0f))
	, mFrontDir(Vector3(0.0f, 0.0f, 1.0f))
	, mUpDir(Vector3(0.0f, 1.0f, 0.0f))
	, mRightDir(Vector3(1.0f, 0.0f, 0.0f))
	, mPitch(0.0f)
	, mYaw(0.0f)
	, mProjFOVAngleY(70.f)
	, mNearZ(0.01f)
	, mFarZ(100.0f)
	, mResolution(1280.f, 720.f)
	, mAspect(mResolution.x / mResolution.y)
	, mPixelsPerUnit(0.f)
	, mViewType(Viewtype::Orthographic)
	, mZoomDelta(0.f)
	, mZoomFactor(1.0f)
{
}

Camera::~Camera() {}

FTWindow* Camera::GetRenderWindow() const
{
	return mRenderWindow;
}

Matrix Camera::GetViewRow()
{
	if (mTarget)
	{
		Transform* transform = mTarget->GetTransform();
		FTVector3  targetPos = transform->GetWorldPosition();
		// Z axis transformation is controlled independently
		FTVector3 camPos = FTVector3(targetPos.x, targetPos.y, mPosition.z);
		mPosition		 = (camPos + mOffset).GetDXVec3();
		mYaw			 = -transform->GetWorldRotation().y;
		mPitch			 = transform->GetWorldRotation().x;
	}

	return Matrix::CreateTranslation(-mPosition) * Matrix::CreateRotationY(-mYaw) * Matrix::CreateRotationX(mPitch);
}

Matrix Camera::GetProjRow()
{
	float		unitsPerPixel = 1 / mPixelsPerUnit;
	FTRectArea* renderArea	  = mRenderWindow->GetRenderArea();
	FTVector2	renderSize	  = renderArea->GetSize();

	float worldWidth  = renderSize.x * unitsPerPixel;
	float worldHeight = renderSize.y * unitsPerPixel;

	worldWidth /= mZoomFactor;
	worldHeight /= mZoomFactor;

	mAspect = renderSize.x / renderSize.y;

	return mViewType == Viewtype::Perspective
		? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mProjFOVAngleY), mAspect, mNearZ, mFarZ)
		: DirectX::XMMatrixOrthographicLH(
			  worldWidth, worldHeight, mNearZ, mFarZ);
}

const FTVector3& Camera::GetPosition() const
{
	return mPosition;
}

FTVector2& Camera::GetResolution() const
{
	return mRenderWindow->GetRenderArea()->GetSize();
}

Viewtype Camera::GetViewType()
{
	return mViewType;
}

float Camera::GetProjFOVAngleY()
{
	return mProjFOVAngleY;
}

float Camera::GetAspectRatio()
{
	return mAspect;
}

float Camera::GetPixelsPerUnit()
{
	return mPixelsPerUnit;
}

float Camera::GetNearZ()
{
	return mNearZ;
}

float Camera::GetFarZ()
{
	return mFarZ;
}

const FTVector3& Camera::GetOffSet() const
{
	return mOffset;
}

Vector3& Camera::Position()
{
	return mPosition;
}

Vector3& Camera::FrontDir()
{
	return mFrontDir;
}

Vector3& Camera::UpDir()
{
	return mUpDir;
}

Vector3& Camera::RightDir()
{
	return mRightDir;
}

float& Camera::ZoomDelta()
{
	return mZoomDelta;
}

void Camera::SetPosition(FTVector3 pos)
{
	mPosition = pos.GetDXVec3();
}

void Camera::SetTargetActor(Actor* actor)
{
	mTarget = actor;
}

void Camera::SetViewType(Viewtype viewType)
{
	mViewType = viewType;
}

void Camera::SetOffset(FTVector3 offset)
{
	mOffset = offset;
}

void Camera::InitializePixelsPerUnit(UINT pixels, float units)
{
	mPixelsPerUnit = (float)pixels / units;
}

FTVector3 Camera::ConvertToCenter(FTVector3 topLeftPos, FTVector2 renderSize)
{
	FTVector3 pos			= topLeftPos * FTVector3(-1.f, 1.f, 1.0f);
	float	  unitsPerPixel = 1 / mPixelsPerUnit;
	float	  worldWidth	= renderSize.x * unitsPerPixel;
	float	  worldHeight	= renderSize.y * unitsPerPixel;
	pos += FTVector3(worldWidth / 2, worldHeight / 2, 0.f);
	return pos;
}

FTVector3 Camera::ConvertToTopLeft(FTVector3 centerPos, FTVector2 renderSize)
{
	float	  unitsPerPixel = 1 / mPixelsPerUnit;
	float	  worldWidth	= renderSize.x * unitsPerPixel;
	float	  worldHeight	= renderSize.y * unitsPerPixel;
	FTVector3 pos			= centerPos - FTVector3(worldWidth / 2, worldHeight / 2, 0.f);
	pos						= pos * FTVector3(-1.f, 1.f, 1.0f);
	return pos;
}

void Camera::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CAMERA_DATA);
	if (mTarget)
		FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, mTarget->GetNameRef());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, ChunkKey::NullVal::NULL_OBJECT);
	FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_POSITION, mPosition);
	FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_OFFSET, mOffset);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CAMERA_DATA);
}

void Camera::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CAMERA_DATA);
	FileIOHelper::LoadVector3(ifs, mOffset);

	FTVector3 pos = FTVector3::Zero;
	FileIOHelper::LoadVector3(ifs, pos);
	mPosition				 = pos.GetDXVec3();
	FTDS::String targetActor = {};
	FileIOHelper::LoadBasicString(ifs, targetActor);

#ifdef FOXTROT_EDITOR
	if (targetActor.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		mTarget = EditorSceneManager::GetInstance()->GetEditorScene()->FindEditorElement(targetActor, nullptr);
#else
	if (targetActor != ChunkKey::NullVal::NULL_OBJECT)
		mTarget = SceneManager::GetInstance()->GetCurrentScene()->FindActor(targetActor);
#endif // FOXTROT_EDITOR
}

FTVector3 Camera::ConvertScreenPosToWorld(FTVector2 screenPos)
{
	FTVector2 ndc = ConvertScreenPosToNDC(screenPos);

	Vector4 clipSpacePos = Vector4(ndc.x, ndc.y, 0.0f, 1.0f);

	Vector4 viewSpacePos = Vector4::Transform(
		clipSpacePos,
		GetProjRow().Invert());

	viewSpacePos /= viewSpacePos.w;

	Vector4 worldSpacePos = Vector4::Transform(
		viewSpacePos,
		GetViewRow().Invert());

	return FTVector3(worldSpacePos.x, worldSpacePos.y, worldSpacePos.z);
}

FTVector2 Camera::ConvertScreenPosToNDC(FTVector2 screenPos)
{
	FTRectArea* renderArea = mRenderWindow->GetRenderArea();
	FTVector2	renderSize = renderArea->GetSize();

	FTVector2 ndc;
	ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
	ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
	return ndc;
}