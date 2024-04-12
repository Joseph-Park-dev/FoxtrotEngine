#include "Transform.h"
#include "CCore.h"
#include "Camera2D.h"

void Transform::Translate(Vector2 translation)
{
	//if (mWorldPosition.x < 0.0f) { mWorldPosition.x = 1022.0f; }
	//else if (mWorldPosition.x > 1024.0f) { mWorldPosition.x = 2.0f; }

	//if (mWorldPosition.y < 0.0f) { mWorldPosition.y = 766.0f; }
	//else if (mWorldPosition.y > 768.0f) { mWorldPosition.y = 2.0f; }

	mWorldPosition = mWorldPosition + translation;
	/*if (mParent != nullptr)
		mWorldPosition + mLocalPosition;*/
}

const Vector2 Transform::GetScreenPosition() const
{
	return Camera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
}

#ifdef _DEBUG
#include "EditorCamera2D.h"

const Vector2 Transform::GetScreenPositionEditor() const
{
	return EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(mWorldPosition);
}
#endif // _DEBUG

Transform::Transform()
	: mWorldPosition(Vector2::Zero)
	, mScreenPosition(Vector2::Zero)
	, mLocalPosition(Vector2::Zero)
	, mScale(Vector2(1.0f, 1.0f))
	, mRotation(0.0f)
{}