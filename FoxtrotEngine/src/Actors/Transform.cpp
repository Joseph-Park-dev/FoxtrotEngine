#include "Actors/Transform.h"

#include <nlohmann/json.hpp>

#include "FileSystem/ChunkLoader.h"

void Transform::Translate(FTVector3 translation)
{
	//if (mWorldPosition.x < 0.0f) { mWorldPosition.x = 1022.0f; }
	//else if (mWorldPosition.x > 1024.0f) { mWorldPosition.x = 2.0f; }

	//if (mWorldPosition.y < 0.0f) { mWorldPosition.y = 766.0f; }
	//else if (mWorldPosition.y > 768.0f) { mWorldPosition.y = 2.0f; }

	mWorldPosition = mWorldPosition + translation;
	/*if (mParent != nullptr)
		mWorldPosition + mLocalPosition;*/
}

//const FTVector2 Transform::GetScreenPosition() const
//{
//	return Camera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
//}
//
//
//
//
//const FTVector2 Transform::GetScreenPositionEditor() const
//{
//	return EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
//}
//#endif // _DEBUG

void Transform::SetCurrentDirection(int dir)
{
	if (dir != 0)
		mCurrentDirection = dir;
}

Transform::Transform()
	: mWorldPosition	(FTVector3::Zero)
	, mScreenPosition	(FTVector3::Zero)
	, mLocalPosition	(FTVector3::Zero)
	, mScale			(FTVector3(1.0f, 1.0f, 1.0f))
	, mRotation			(FTVector3(0.0f, 0.0f, 0.0f))
	, mCurrentDirection	(1)
{}

Transform::Transform(Transform & origin)
	: mWorldPosition	(origin.mWorldPosition)
	, mScreenPosition	(origin.mScreenPosition)
	, mLocalPosition	(origin.mLocalPosition)
	, mScale			(origin.mScale)
	, mRotation			(origin.mRotation)
{}

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

void Transform::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddVector3(out[SAVEKEY_WORLDPOS], mWorldPosition);
	FileIOHelper::AddVector3(out[SAVEKEY_SCREENPOS], mScreenPosition);
	FileIOHelper::AddVector3(out[SAVEKEY_LOCALPOS], mLocalPosition);
	FileIOHelper::AddVector3(out[SAVEKEY_SCALE], mScale);
	FileIOHelper::AddVector3(out[SAVEKEY_ROTATION], mRotation);
}

void Transform::LoadProperties()
{
}
