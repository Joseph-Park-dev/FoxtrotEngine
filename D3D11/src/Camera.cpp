// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Camera.h"

#include "Renderer/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "InputSystem/FTInputDevice.h"
#include "Manager/SceneManager.h"
#include "Scene/Scene.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "Actor/Transform.h"
#include "Actor/Actor.h"
#include "FileSystem/NullKeys.h"
#include "Math/FTMath.h"

#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

void Camera::Initialize(FTWindow* renderWindow, unsigned int pixels, float unit)
{
	assert(!mRenderWindow); // Overlapped assignment must be prevented.

	mRenderWindow = renderWindow;
	mAspect		  = GetResolution().x / GetResolution().y;
	InitializePixelsPerUnit(pixels, unit);
}

void Camera::Update(float deltaTime)
{
	// Zoom();
}

void Camera::Zoom()
{
	Math::Clampf(mZoomFactor, 0.01f, 10.0f);
	mZoomFactor += mZoomDelta;
}

Camera::Camera()
	: mRenderWindow(nullptr)
	, mTarget(nullptr)
	, mPosition(FTVector3(0.0f, 0.0f, -5.0f))
	, mProjFOVAngleY(70.f)
	, mNearZ(0.01f)
	, mFarZ(100.0f)
	, mAspect(0.f)
	, mPixelsPerUnit(0.f)
	, mZoomDelta(0.f)
	, mZoomFactor(1.0f)
	, mViewType(Viewtype::Orthographic)
{
}

Camera::~Camera() {}

FTMatrix4 Camera::GetViewRow()
{
	FTVector3 camPos = FTVector3::Zero;
	if (mTarget)
	{
		Transform* transform = mTarget->GetTransform();
		FTVector3  targetPos = transform->GetWorldPosition();
		// Z axis transformation is controlled independently
		camPos	  = FTVector3(targetPos.x, targetPos.y, mPosition.z);
		mPosition = (camPos + mOffset);
	}

	return FTMatrix4::CreateTranslation(mPosition);
}

FTMatrix4 Camera::GetProjRow()
{
	float	  unitsPerPixel = 1 / mPixelsPerUnit;
	FTVector2 renderSize	= GetResolution();
	assert(0 < renderSize.x);
	assert(0 < renderSize.y);

	float worldWidth  = renderSize.x * unitsPerPixel;
	float worldHeight = renderSize.y * unitsPerPixel;

	worldWidth /= mZoomFactor;
	worldHeight /= mZoomFactor;

	mAspect = renderSize.x / renderSize.y;

	return mViewType == Viewtype::Perspective
		? FTMatrix4::CreatePerspectiveFOV(Math::ToRadians(mProjFOVAngleY), GetResolution().x, GetResolution().y, mNearZ, mFarZ)
		: FTMatrix4::CreateOrtho(
			  worldWidth, worldHeight, mNearZ, mFarZ);
}

const FTVector3& Camera::GetPosition() const
{
	return mPosition;
}

const FTVector2& Camera::GetResolution() const
{
	return mRenderWindow->GetRenderArea()->GetSize();
}

const Viewtype Camera::GetViewType()
{
	return mViewType;
}

const float Camera::GetProjFOVAngleY()
{
	return mProjFOVAngleY;
}

const float Camera::GetAspectRatio()
{
	return mAspect;
}

const float Camera::GetPixelsPerUnit()
{
	return mPixelsPerUnit;
}

const float Camera::GetNearZ()
{
	return mNearZ;
}

const float Camera::GetFarZ()
{
	return mFarZ;
}

const FTVector3& Camera::GetOffSet() const
{
	return mOffset;
}

const float Camera::GetZoomFactor() const
{
	return mZoomFactor;
}

FTVector3& Camera::Offset()
{
	return mOffset;
}

float& Camera::ZoomDelta()
{
	return mZoomDelta;
}

void Camera::SetPosition(FTVector3 pos)
{
	mPosition = pos;
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

void Camera::InitializePixelsPerUnit(unsigned int pixels, float units)
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

//void Camera::ConvertWorldPosToScreen(const FTVector3& worldPos, const Transform* transform, FTVector2& out)
//{
//	// origin			 = FTVector3::Transform(worldPos, transform->GetMatrixWorld());
//	// origin			 = FTVector3::Transform(origin, camInst->GetViewRow());
//	// origin			 = FTVector3::Transform(origin, camInst->GetProjRow());
//
//	// origin.x /= origin.w;
//	// origin.y /= origin.w;
//	// origin.z /= origin.w;
//
//	// float screenX = static_cast<float>((origin.x + 1) * 0.5 * renderSize.x);
//	// float screenY = static_cast<float>((1 - origin.y) * 0.5 * renderSize.y);
//
//	// return FTVector2(screenX, screenY);
//}

void Camera::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CAMERA_DATA);
	if (mTarget)
		FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, mTarget->GetNameRef());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, ChunkKey::NullVal::NULL_OBJECT);
	FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_POSITION, mPosition);
	FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_OFFSET, mOffset);
	FileIOHelper::SaveFloat(ofs, ChunkKey::CAM_ZOOM, mZoomFactor);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CAMERA_DATA);
}

void Camera::LoadProperties(std::ifstream& ifs, SceneManager* manager)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CAMERA_DATA);
	FileIOHelper::LoadFloat(ifs, mZoomFactor);
	FileIOHelper::LoadVector3(ifs, mOffset);

	FileIOHelper::LoadVector3(ifs, mPosition);
	FTDS::String targetName = {};
	FileIOHelper::LoadBasicString(ifs, targetName);

#ifdef FOXTROT_EDITOR
	if (targetActor.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		mTarget = EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(targetActor, nullptr);
#else
	if (targetName.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		mTarget = manager->GetCurrentScene()->FindActor(targetName);
#endif // FOXTROT_EDITOR
}

FTVector3 Camera::ConvertScreenPosToWorld(FTVector2 screenPos)
{
	FTVector2 ndc = ConvertScreenPosToNDC(screenPos);
	FTVector3 clipPos = FTVector3(ndc.x, ndc.y, 0.0f);

	FTMatrix4 viewProj = GetViewRow() * GetProjRow();
	viewProj.Invert();

	return FTVector3::Transform(clipPos, viewProj, 1.0f);
}

FTVector2 Camera::ConvertScreenPosToNDC(FTVector2 screenPos)
{
	FTVector2 renderSize = GetResolution();
	FTVector2 ndc		 = FTVector2::Zero;

	ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
	ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
	return ndc;
}

#ifdef FOXTROT_EDITOR
float& Camera::ZoomFactor()
{
	return mZoomFactor;
}
#endif // FOXTROT_EDITOR
