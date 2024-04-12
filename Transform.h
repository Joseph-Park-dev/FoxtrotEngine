#pragma once
#include "Math.h"
#include "Component.h"

class Transform
{
public:
	void Translate(Vector2 translation);

public:
	const Vector2  GetWorldPosition()		 const { return mWorldPosition; }
	const Vector2  GetLocalPosition()		 const { return mLocalPosition; }
	const Vector2  GetForward()				 const { return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation)); }
	const Vector2  GetScale()				 const { return mScale; }
	const float    GetRotation()			 const { return mRotation; }
	const Vector2  GetScreenPosition()		 const;
#ifdef _DEBUG
	const Vector2  GetScreenPositionEditor() const;
#endif

	Vector2* GetWorldPositionRef()  { return &mWorldPosition; }
	Vector2* GetLocalPositionRef()  { return &mLocalPosition; }
	Vector2* GetScaleRef()			{ return &mScale; }
	float*	 GetRotationRef()		{ return &mRotation; }

	void	 SetWorldPosition (const Vector2 pos) { mWorldPosition = pos; }
	//void	 SetWorldPositionBox2D (const b2Vec2 pos) { mWorldPosition = Vector2(pos.x, pos.y); }
	void	 SetScreenPosition(const Vector2 pos) { mScreenPosition = pos; }
	void	 SetLocalPosition (Vector2 pos)		 { mLocalPosition = pos; }
	void	 SetScale		    (Vector2 scale)		 { mScale = scale; }
	void	 SetRotation	    (float rotation)	 { mRotation = Math::ToRadians(rotation); }

public:
	Transform();

private:
	Vector2		 mWorldPosition;
	Vector2		 mScreenPosition;
	Vector2		 mLocalPosition;
	Vector2		 mScale;
	float		 mRotation;
};