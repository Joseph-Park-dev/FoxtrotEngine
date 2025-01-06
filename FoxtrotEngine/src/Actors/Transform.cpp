// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actors/Transform.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

const FTVector3 Transform::GetWorldPosition() const
{
	return mWorldPosition;
}

const FTVector3 Transform::GetLocalPosition() const
{
	return mLocalPosition;
}

const FTVector3 Transform::GetScale() const
{
	return mScale;
}

const FTVector3 Transform::GetRotation() const
{
	return mRotation;
}

const FTVector3 Transform::GetRotationDegree() const
{
	return ConvertRadToDegree(mRotation);
}

const FTVector3 Transform::GetRightward() const
{
	return mRightward;
}

const Steering* Transform::GetSteering() const
{
	return mSteering;
}

void Transform::SetWorldPosition(const FTVector3 pos)
{
	mWorldPosition = pos;
}

void Transform::SetLocalPosition(FTVector3 pos)
{
	mLocalPosition = pos;
}

void Transform::SetScale(FTVector3 scale)
{
	mScale = scale;
}

void Transform::SetRotation(FTVector3 rotation)
{
	mRotation = rotation;
}

void Transform::SetRightward(FTVector3 dir)
{
	assert(mRightward != FTVector3::Zero);
	mRightward = dir;
}

void Transform::SetSteering(Steering steering)
{
	if (!mSteering)
		mSteering = Steering::CreateEmptySteering();
	mSteering->Angular			= steering.Angular;
	mSteering->Linear			= steering.Linear;
	mSteering->JumpTriggered	= steering.JumpTriggered;
}

Transform::Transform()
	: mWorldPosition		(FTVector3::Zero)
	, mLocalPosition		(FTVector3::Zero)
	, mScale				(FTVector3(1.0f, 1.0f, 1.0f))
	, mRotation				(FTVector3(0.0f, 0.0f, 0.0f))
	, mRightward			(FTVector3(1.0f, 0.0f, 0.0f))
	, mSteering				(nullptr)
{}

Transform::Transform(Transform & origin)
	: mWorldPosition		(origin.mWorldPosition)
	, mLocalPosition		(origin.mLocalPosition)
	, mScale				(origin.mScale)
	, mRotation				(origin.mRotation)
	, mRightward			(origin.mRightward)
	, mSteering				(nullptr)
{}

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

void Transform::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::TRANSFORM);
	FileIOHelper::SaveVector3(ofs, ChunkKeys::WORLD_POS, mWorldPosition);
	FileIOHelper::SaveVector3(ofs, ChunkKeys::LOCAL_POS, mLocalPosition);
	FileIOHelper::SaveVector3(ofs, ChunkKeys::SCALE,	 mScale);
	FileIOHelper::SaveVector3(ofs, ChunkKeys::ROTATION,  mRotation);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::TRANSFORM);
}

void Transform::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::TRANSFORM);
	FileIOHelper::LoadVector3(ifs, mRotation);
	FileIOHelper::LoadVector3(ifs, mScale);
	FileIOHelper::LoadVector3(ifs, mLocalPosition);
	FileIOHelper::LoadVector3(ifs, mWorldPosition);
}