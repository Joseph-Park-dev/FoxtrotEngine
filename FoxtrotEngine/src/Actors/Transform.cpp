// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actors/Transform.h"

#include "directxtk/SimpleMath.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/vertex.h"
#include "Renderer/Camera.h"
#include "Renderer/FTRectArea.h"
#include "WindowSystem/FTWindow.h"

using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix  = DirectX::SimpleMath::Matrix;

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

const FTVector3& Transform::GetLocalPosition() const { return mLocalPosition; }
const FTVector3& Transform::GetLocalScale() const { return mLocalScale; }
const FTVector3& Transform::GetLocalRotation() const { return mLocalRotation; }

const FTVector3&				   Transform::GetWorldPosition() const { return mWorldPosition; }
const FTVector3&				   Transform::GetWorldScale() const { return mWorldScale; }
const FTVector3&				   Transform::GetWorldRotation() const { return mWorldRotation; }
const DirectX::SimpleMath::Matrix& Transform::GetMatrixWorld() const { return mMatrixWorld; }

const FTVector2 Transform::GetScreenPosition(Camera* camInst) const
{
	FTWindow*	window	   = camInst->GetRenderWindow();
	FTRectArea* renderArea = window->GetRenderArea();
	FTVector2	renderSize = renderArea->GetSize();
	FTVector3	worldPos   = GetWorldPosition();

	Vector4 origin = Vector4::Zero;
	origin.w	   = 1.0f;
	origin		   = Vector4::Transform(origin, mMatrixWorld);
	origin		   = Vector4::Transform(origin, camInst->GetViewRow());
	origin		   = Vector4::Transform(origin, camInst->GetProjRow());

	origin.x /= origin.w;
	origin.y /= origin.w;
	origin.z /= origin.w;

	float screenX = (origin.x + 1) * 0.5 * renderSize.x;
	float screenY = (1 - origin.y) * 0.5 * renderSize.y;

	return FTVector2(screenX, screenY);
}

const FTVector3& Transform::GetRotationDegree() const
{
	return ConvertRadToDegree(mWorldRotation);
}

const FTVector3& Transform::GetRightward() const
{
	return mRightward;
}

const Steering* Transform::GetSteering() const
{
	return mSteering;
}

const bool Transform::IsHalting() const
{
	return mSteering->Linear == FTVector3::Zero && !mSteering->JumpTriggered;
}

void Transform::SetLocalPosition(const FTVector3 localPos)
{
	mLocalPosition = localPos;
	Update();
}

void Transform::SetLocalScale(const FTVector3 localScale)
{
	mLocalScale = localScale;
	Update();
}

void Transform::SetLocalRotation(const FTVector3 localRot)
{
	mLocalRotation = localRot;
	Update();
}

void Transform::SetWorldPosition(const FTVector3 worldPos)
{
	mWorldPosition = worldPos;
	Actor* parent  = mOwner->GetParent();
	if (!parent)
		SetLocalPosition(worldPos);
}

void Transform::SetWorldScale(const FTVector3 worldScale)
{
	mWorldScale	  = worldScale;
	Actor* parent = mOwner->GetParent();
	if (!parent)
		SetLocalScale(worldScale);
}

void Transform::SetWorldRotation(const FTVector3 worldRot)
{
	mWorldRotation = worldRot;
	Actor* parent  = mOwner->GetParent();
	if (!parent)
		SetLocalRotation(worldRot);
}

void Transform::SetRightward(FTVector3 dir)
{
	assert(mRightward != FTVector3::Zero);
	mRightward = dir;
}

void Transform::SetSteering(Steering steering)
{
	mSteering->Angular		 = steering.Angular;
	mSteering->Linear		 = steering.Linear;
	mSteering->JumpTriggered = steering.JumpTriggered;
}

void Transform::SetOwner(Actor* actor)
{
	mOwner = actor;
}

Transform::Transform(Actor* owner)
	: mLocalPosition(FTVector3::Zero)
	, mLocalScale(FTVector3(1.f, 1.0f, 1.0f))
	, mLocalRotation(FTVector3::Zero)

	, mMatrixLocal(DirectX::SimpleMath::Matrix::Identity)
	, mMatrixWorld(DirectX::SimpleMath::Matrix::Identity)

	, mWorldPosition(FTVector3::Zero)
	, mWorldScale(FTVector3(1.0f, 1.0f, 1.0f))
	, mWorldRotation(FTVector3(0.0f, 0.0f, 0.0f))

	, mRightward(FTVector3(1.0f, 0.0f, 0.0f))
	, mSteering(Steering::CreateEmptySteering())

	, mOwner(owner)
{
}

Transform::Transform(Actor* owner, Transform& origin)
	: mLocalPosition(origin.mLocalPosition)
	, mLocalScale(origin.mLocalScale)
	, mLocalRotation(origin.mLocalRotation)

	, mMatrixLocal(origin.mMatrixLocal)
	, mMatrixWorld(origin.mMatrixWorld)

	, mWorldPosition(origin.mWorldPosition)
	, mWorldScale(origin.mWorldScale)
	, mWorldRotation(origin.mWorldRotation)

	, mRightward(origin.mRightward)
	, mSteering(Steering::CreateEmptySteering())

	, mOwner(owner)
{
}

Transform::~Transform()
{
	if (mSteering)
	{
		delete mSteering;
		mSteering = nullptr;
	}
}

FTVector3 Transform::ConvertRadToDegree(FTVector3 radianRot)
{
	float x = Math::ToDegrees(radianRot.x);
	float y = Math::ToDegrees(radianRot.y);
	float z = Math::ToDegrees(radianRot.z);
	return FTVector3(x, y, z);
}

FTVector3 Transform::ConvertDegreeToRad(FTVector3 degreeRot)
{
	float x = Math::ToRadians(degreeRot.x);
	float y = Math::ToRadians(degreeRot.y);
	float z = Math::ToRadians(degreeRot.z);
	return FTVector3(x, y, z);
}

void Transform::Update()
{
	Matrix matScale	   = Matrix::CreateScale(mLocalScale.GetDXVec3());
	Matrix matRotation = Matrix::CreateRotationX(mLocalRotation.x) *
		Matrix::CreateRotationY(mLocalRotation.y) *
		Matrix::CreateRotationZ(mLocalRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(mLocalPosition.GetDXVec3());

	mMatrixLocal = matScale * matRotation * matTranslation;

	Actor* parent = mOwner->GetParent();
	if (parent)
		mMatrixWorld = mMatrixLocal * parent->GetTransform()->mMatrixWorld;
	else
		mMatrixWorld = mMatrixLocal;

	FTDS::DynamicArray<Actor*>& childActors = mOwner->GetChildActors();
	if (0 < childActors.GetSize())
	{
		for (auto child = childActors.Begin(); child != childActors.End(); ++child)
			(*child)->GetTransform()->Update();
	}
	// FTVector3::DecomposeMatrix(mWorldScale, mWorldRotation, mWorldPosition, mMatrixWorld);
}

void Transform::CloneTo(Transform* target)
{
	target->mOwner = mOwner;

	if (mOwner->GetParent())
	{
		target->SetLocalPosition(mLocalPosition);
		target->SetLocalScale(mLocalScale);
		target->SetLocalRotation(mLocalRotation);
	}
	else
	{
		target->SetWorldPosition(mWorldPosition);
		target->SetWorldScale(mWorldScale);
		target->SetWorldRotation(mWorldRotation);
	}
}

void Transform::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::TRANSFORM);
	FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_ROTATION, mWorldRotation);
	FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_SCALE, mWorldScale);
	FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_POS, mWorldPosition);

	FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_ROTATION, mLocalRotation);
	FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_SCALE, mLocalScale);
	FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_POS, mLocalPosition);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::TRANSFORM);
}

void Transform::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::TRANSFORM);
	FileIOHelper::LoadVector3(ifs, mLocalPosition);
	FileIOHelper::LoadVector3(ifs, mLocalScale);
	FileIOHelper::LoadVector3(ifs, mLocalRotation);

	FileIOHelper::LoadVector3(ifs, mWorldPosition);
	FileIOHelper::LoadVector3(ifs, mWorldScale);
	FileIOHelper::LoadVector3(ifs, mWorldRotation);

	SetWorldPosition(mWorldPosition);
	SetWorldScale(mWorldScale);
	SetWorldRotation(mWorldRotation);
}

#ifdef FOXTROT_EDITOR
void Transform::UpdateUI()
{
	if (!mOwner->GetParent()) // The owner does not have parent Actor.
	{
		FTVector3 worldPos = mWorldPosition;
		FTVector3 worldScale = mWorldScale;
		FTVector3 worldRot = mWorldRotation;

		CommandHistory::GetInstance()->UpdateVector3Value("World Position", worldPos);
		CommandHistory::GetInstance()->UpdateVector3Value("World Scale", worldScale);
		CommandHistory::GetInstance()->UpdateVector3Value("World Rotation", worldRot);

		SetWorldPosition(worldPos);
		SetWorldScale(worldScale);
		SetWorldRotation(worldRot);
	}
	else
	{
		FTVector3 localPos = mLocalPosition;
		FTVector3 localScale = mLocalScale;
		FTVector3 localRot = mLocalRotation;

		CommandHistory::GetInstance()->UpdateVector3Value("Local Position", localPos);
		CommandHistory::GetInstance()->UpdateVector3Value("Local Scale", localScale);
		CommandHistory::GetInstance()->UpdateVector3Value("Local Rotation", localRot);

		SetLocalPosition(localPos);
		SetLocalScale(localScale);
		SetLocalRotation(localRot);
	}
}
#endif // FOXTROT_EDITOR