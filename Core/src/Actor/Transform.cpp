// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor/Transform.h"

#include "Actor/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/FTWindow.h"
#include "Math/FTMath.h"
#include "Steering.h"
#include "Dynamic/DynamicArray.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

const FTVector3& Transform::GetLocalPosition() const { return mLocalPosition; }
const FTVector3& Transform::GetLocalScale() const { return mLocalScale; }
const FTVector3& Transform::GetLocalRotation() const { return mLocalRotation; }

const FTVector3& Transform::GetWorldPosition() const { return mWorldPosition; }
const FTVector3& Transform::GetWorldScale() const { return mWorldScale; }
const FTVector3& Transform::GetWorldRotation() const { return mWorldRotation; }
const FTMatrix4& Transform::GetMatrixWorld() const { return mMatrixWorld; }

const FTVector3 Transform::GetRotationDegree() const
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
	, mMatrixLocal(FTMatrix4::Identity)

	, mWorldPosition(FTVector3::Zero)
	, mWorldScale(FTVector3(1.0f, 1.0f, 1.0f))
	, mWorldRotation(FTVector3(0.0f, 0.0f, 0.0f))
	, mMatrixWorld(FTMatrix4::Identity)

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

	, mWorldPosition(origin.mWorldPosition)
	, mWorldScale(origin.mWorldScale)
	, mWorldRotation(origin.mWorldRotation)
	, mMatrixWorld(origin.mMatrixWorld)

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
	FTMatrix4 matScale	  = FTMatrix4::CreateScale(mLocalScale);
	FTMatrix4 matRotation = FTMatrix4::CreateRotationX(mLocalRotation.x) *
		FTMatrix4::CreateRotationY(mLocalRotation.y) *
		FTMatrix4::CreateRotationZ(mLocalRotation.z);
	FTMatrix4 matTranslation = FTMatrix4::CreateTranslation(mLocalPosition);

	mMatrixLocal = matScale * matRotation * matTranslation;

	Actor* parent = mOwner->GetParent();
	if (parent)
		mMatrixWorld = mMatrixLocal * parent->GetTransform()->mMatrixWorld;
	else
		mMatrixWorld = mMatrixLocal;

	FTDS::DynamicArray<Actor*>* childActors = mOwner->GetChildActors();
	if (0 < childActors->GetSize())
	{
		for (auto child = childActors->Begin(); child != childActors->End(); ++child)
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
		CommandHistory::GetInstance()->UpdateVector3Value("World Position", mWorldPosition);
		CommandHistory::GetInstance()->UpdateVector3Value("World Scale", mWorldScale);
		CommandHistory::GetInstance()->UpdateVector3Value("World Rotation", mWorldRotation);

		SetWorldPosition(mWorldPosition);
		SetWorldScale(mWorldScale);
		SetWorldRotation(mWorldRotation);
	}
	else
	{
		CommandHistory::GetInstance()->UpdateVector3Value("Local Position", mLocalPosition);
		CommandHistory::GetInstance()->UpdateVector3Value("Local Scale", mLocalScale);
		CommandHistory::GetInstance()->UpdateVector3Value("Local Rotation", mLocalRotation);

		SetLocalPosition(mLocalPosition);
		SetLocalScale(mLocalScale);
		SetLocalRotation(mLocalRotation);
	}
}
#endif // FOXTROT_EDITOR