#include "FoxtrotEngine/Actors/Transform.h"

#include "FoxtrotEngine/Renderer/Camera2D.h"

void Transform::Translate(FTVector2 translation)
{
	//if (mWorldPosition.x < 0.0f) { mWorldPosition.x = 1022.0f; }
	//else if (mWorldPosition.x > 1024.0f) { mWorldPosition.x = 2.0f; }

	//if (mWorldPosition.y < 0.0f) { mWorldPosition.y = 766.0f; }
	//else if (mWorldPosition.y > 768.0f) { mWorldPosition.y = 2.0f; }

	mWorldPosition = mWorldPosition + translation;
	/*if (mParent != nullptr)
		mWorldPosition + mLocalPosition;*/
}

const FTVector2 Transform::GetScreenPosition() const
{
	return Camera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
}

#ifdef _DEBUG
#include "FoxtrotEditor/EditorCamera2D.h"

const FTVector2 Transform::GetScreenPositionEditor() const
{
	return EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
}
#endif // _DEBUG

Transform::Transform()
	: mWorldPosition(FTVector2::Zero)
	, mScreenPosition(FTVector2::Zero)
	, mLocalPosition(FTVector2::Zero)
	, mScale(FTVector2(1.0f, 1.0f))
	, mRotation(0.0f)
{}

Transform::Transform(Transform & origin)
	: mWorldPosition(origin.mWorldPosition)
	, mScreenPosition(origin.mScreenPosition)
	, mLocalPosition(origin.mLocalPosition)
	, mScale(origin.mScale)
	, mRotation(origin.mRotation)
{}
