#pragma once
#include "FoxtrotEngine/Math/FTMath.h"

class Component;

class Transform
{
public:
	void Translate(FTVector2 translation);

public:
	const FTVector3  GetWorldPosition()	 const { return mWorldPosition; }
	const FTVector3  GetLocalPosition()	 const { return mLocalPosition; }
	//const FTVector3  GetForward()		 const { return FTVector3(Math::Cos(mRotation), -Math::Sin(mRotation)); }
	const FTVector3  GetScale()			 const { return mScale; }
	const FTVector3	 GetRotation()		 const { return mRotation; }
	const FTVector3  GetScreenPosition() const;
#ifdef _DEBUG
	const FTVector2 GetScreenPositionEditor() const;
#endif

	FTVector3*	GetWorldPositionRef()	{ return &mWorldPosition; }
	FTVector3*	GetLocalPositionRef()	{ return &mLocalPosition; }
	FTVector3*	GetScaleRef()			{ return &mScale; }
	FTVector3*	GetRotationRef()		{ return &mRotation; }

	void	 SetWorldPosition	(const FTVector3 pos)	{ mWorldPosition = pos; }
	//void	 SetWorldPositionBox2D (const b2Vec2 pos)	{ mWorldPosition = FTVector2(pos.x, pos.y); }
	void	 SetScreenPosition	(const FTVector3 pos)	{ mScreenPosition = pos; }
	void	 SetLocalPosition	(FTVector3 pos)			{ mLocalPosition = pos; }
	void	 SetScale		    (FTVector3 scale)		{ mScale = scale; }
	void	 SetRotation(FTVector3 rotation)			{ mRotation = rotation; }

public:
	Transform();
	Transform(Transform& origin);

private:
	FTVector3	mWorldPosition;
	FTVector3	mScreenPosition;
	FTVector3	mLocalPosition;
	FTVector3	mScale;
	FTVector3	mRotation;
};