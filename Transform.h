#pragma once
#include "Math.h"
#include "Component.h"

class Transform
{
public:
	void Translate(FTVector2 translation);

public:
	const FTVector2  GetWorldPosition()	 const { return mWorldPosition; }
	const FTVector2  GetLocalPosition()	 const { return mLocalPosition; }
	const FTVector2  GetForward()		 const { return FTVector2(Math::Cos(mRotation), -Math::Sin(mRotation)); }
	const FTVector2  GetScale()			 const { return mScale; }
	const float		 GetRotation()		 const { return mRotation; }
	const FTVector2  GetScreenPosition() const;
#ifdef _DEBUG
	const FTVector2  GetScreenPositionEditor() const;
#endif

	FTVector2* GetWorldPositionRef()  { return &mWorldPosition; }
	FTVector2* GetLocalPositionRef()  { return &mLocalPosition; }
	FTVector2* GetScaleRef()			{ return &mScale; }
	float*	 GetRotationRef()		{ return &mRotation; }

	void	 SetWorldPosition	(const FTVector2 pos) { mWorldPosition = pos; }
	//void	 SetWorldPositionBox2D (const b2Vec2 pos) { mWorldPosition = FTVector2(pos.x, pos.y); }
	void	 SetScreenPosition	(const FTVector2 pos) { mScreenPosition = pos; }
	void	 SetLocalPosition	(FTVector2 pos)		 { mLocalPosition = pos; }
	void	 SetScale		    (FTVector2 scale)		 { mScale = scale; }
	void	 SetRotation	    (float rotation)	 { mRotation = Math::ToRadians(rotation); }

public:
	Transform();

private:
	FTVector2	 mWorldPosition;
	FTVector2	 mScreenPosition;
	FTVector2	 mLocalPosition;
	FTVector2	 mScale;
	float		 mRotation;
};