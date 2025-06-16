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

void Camera::ZoomIn()
{
}

Camera::Camera()
	: mRenderWindow(nullptr)
	, mTarget(nullptr)
	, mPosition(Vector3(0.0f, 0.0f, -5.0f))
	, mFrontDir(Vector3(0.0f, 0.0f, -1.0f))
	, mUpDir(Vector3(0.0f, -1.0f, 0.0f))
	, mRightDir(Vector3(1.0f, 0.0f, 0.0f))
	, mPitch(0.0f)
	, mYaw(0.0f)
	, mProjFOVAngleY(70.f)
	, mNearZ(0.01f)
	, mFarZ(100.0f)
	, mAspect(1280.f / 720.f)
	, mPixelsPerUnit(0.f)
	, mViewType(Viewtype::Perspective)
{
}

Camera::~Camera() {}

Matrix Camera::GetViewRow()
{
	float unitsPerPixel = 1 / mPixelsPerUnit;
	if (mTarget)
	{
		Transform* transform = mTarget->GetTransform();
		mPosition			 = (transform->GetWorldPosition() + mOffset).GetDXVec3();
		mYaw				 = -transform->GetWorldRotation().y;
		mPitch				 = transform->GetWorldRotation().x;
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

	mAspect = renderSize.x / renderSize.y;

	return mViewType == Viewtype::Perspective
		? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mProjFOVAngleY), mAspect, mNearZ, mFarZ)
		: DirectX::XMMatrixOrthographicOffCenterLH(
			  0.0f, worldWidth, worldHeight, 0.0f, mNearZ, mFarZ);
}

Vector3 Camera::GetEyePos()
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

void Camera::SetTargetActor(Actor* actor)
{
	mTarget = actor;
}

void Camera::SetViewType(Viewtype viewType)
{
	mViewType = viewType;
}

void Camera::SetOffset(FTVector2 offset)
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
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CAMERA_DATA);
}

void Camera::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CAMERA_DATA);

	FTVector3 pos = FTVector3::Zero;
	FileIOHelper::LoadVector3(ifs, pos);
	mPosition				 = pos.GetDXVec3();
	FTDS::String targetActor = {};
	FileIOHelper::LoadBasicString(ifs, targetActor);

#ifdef FOXTROT_EDITOR
	if (targetActor.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		mTarget = EditorSceneManager::GetInstance()->GetEditorScene()->FindEditorElement(targetActor, nullptr);
#else
	if (targetActor != ChunkKey::NullVal::NULL_OBJ)
		mTarget = SceneManager::GetInstance()->GetCurrentScene()->FindActor(targetActor);
#endif // FOXTROT_EDITOR
}

FTVector3 Camera::ConvertScreenPosToWorld(FTVector2 screenPos)
{
	FTRectArea* renderArea	 = mRenderWindow->GetRenderArea();
	FTVector2	renderSize	 = renderArea->GetSize();
	float		screenWidth	 = static_cast<float>(renderSize.x);
	float		screenHeight = static_cast<float>(renderSize.y);

	FTVector2 topLeft	  = FTVector2::Zero;
	FTVector2 bottomRight = FTVector2(screenWidth - 1, screenHeight - 1);

	float nx = (screenPos.x / screenWidth) * 2.f - 1.f;
	float ny = 1 - (screenPos.y / screenHeight) * 2.f;
	float z	 = 0.0f;

	DirectX::XMVECTOR ndcPos		  = DirectX::XMVectorSet(nx, ny, z, 1.0f);
	DirectX::XMMATRIX inverseViewProj = DirectX::XMMatrixInverse(nullptr, (GetViewRow() * GetProjRow()));
	DirectX::XMVECTOR worldPos		  = DirectX::XMVector4Transform(ndcPos, inverseViewProj);

	DirectX::SimpleMath::Vector3 vec3(worldPos);
	return FTVector3(vec3.x, vec3.y, vec3.z);
}

FTVector2 Camera::ConvertScreenPosToNDC(FTVector2 screenPos)
{
	FTRectArea* renderArea = mRenderWindow->GetRenderArea();
	FTVector2	renderSize = renderArea->GetSize();

	FTVector2 ndc;
	ndc.x = screenPos.x / renderSize.x * 2 - 1;
	ndc.y = screenPos.y / renderSize.y * 2 - 1;
	return ndc;
}

#ifdef FOXTROT_EDITOR
void Camera::DisplayCameraMenu()
{
	ImGui::Begin("Main Camera");

	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", mPosition, LOOKAT_MODSPEED);

	/*float yaw	= mYaw;
	float pitch = mPitch;*/

	// CommandHistory::GetInstance()->UpdateFloatValue("Look-At Yaw", yaw, LOOKAT_MODSPEED);
	// CommandHistory::GetInstance()->UpdateFloatValue("Look-At Pitch", mPitch, LOOKAT_MODSPEED);

	/*if (yaw != mYaw || pitch != mPitch)
	{
		mYaw = yaw; mPitch = pitch;
		UpdateViewDirections();
	}*/

	// Set Target
	EditorScene*				editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::vector<EditorElement*> editorElems;
	editorElems = EditorSceneManager::GetInstance()->GetEditorScene()->GetEditorElements();
	FTDS::String* actorNames = DBG_NEW FTDS::String[editorElems.size() + 1];
	actorNames[0]			 = "None";
	size_t		  idx		 = 1;
	static size_t currIdx;

	for (size_t i = 0; i < editorElems.size(); ++i)
		actorNames[i] = editorElems.at(i)->GetName();

	const char* comboPreview = actorNames[currIdx].C_Str();
	if (ImGui::BeginCombo(ChunkKey::TARGET_ACTOR, comboPreview))
	{
		for (size_t i = 0; i < idx; ++i)
		{
			if (ImGui::Selectable(actorNames[i].C_Str()))
			{
				currIdx = i;
				if (currIdx == 0)
					mTarget = nullptr;
				else
					mTarget = editorScene->FindEditorElement(actorNames[currIdx], nullptr);
			}
		}
		ImGui::EndCombo();
	}
	delete[] actorNames;
	CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", mOffset, LOOKAT_MODSPEED);

	if (ImGui::Button("2D"))
	{
		FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
		if (GetViewType() == Viewtype::Perspective)
		{
			SetViewType(Viewtype::Orthographic);
			LogString("Orthographic");
		}
		else if (GetViewType() == Viewtype::Orthographic)
		{
			SetViewType(Viewtype::Perspective);
			LogString("Perspective");
		}
	}

	ImGui::End();
}
#endif // FOXTROT_EDITOR