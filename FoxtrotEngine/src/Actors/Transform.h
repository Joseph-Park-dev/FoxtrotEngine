#pragma once
#include <nlohmann/json.hpp>

#include "Math/FTMath.h"

class Component;

class Transform
{
#define SAVEKEY_WORLDPOS "WorldPosition"
#define SAVEKEY_SCREENPOS "ScreenPosition"
#define SAVEKEY_LOCALPOS "LocalPosition"
#define SAVEKEY_SCALE "Scale"
#define SAVEKEY_ROTATION "Rotation"

public:
	void Translate(FTVector3 translation);

public:
	const FTVector3  GetWorldPosition()	   const { return mWorldPosition; }
	const FTVector3  GetLocalPosition()	   const { return mLocalPosition; }
	//const FTVector3  GetForward()		   const { return FTVector3(Math::Cos(mRotation), -Math::Sin(mRotation)); }
	const FTVector3  GetScale()			   const { return mScale; }
	const FTVector3	 GetRotation()		   const { return mRotation; }
	const FTVector3  GetRotationDegree()   const { return ConvertRadToDegree(mRotation); }
	const int		 GetCurrentDirection() const { return mCurrentDirection; }

	// This is used when the positive Y direction should be inverted
	// (e.g. When drawing Debug Geometries)
	const FTVector3	 GetWorldPositionYInverted() const
	{
		FTVector3 vec3 = mWorldPosition;
		vec3.y = -vec3.y;
		return vec3;
	}

	void SetWorldPosition	 (const FTVector3 pos)	{ mWorldPosition = pos; }
	void SetScreenPosition	 (const FTVector3 pos)	{ mScreenPosition = pos; }
	void SetLocalPosition	 (FTVector3 pos)		{ mLocalPosition = pos; }
	void SetScale		     (FTVector3 scale)		{ mScale = scale; }
	void SetRotation		 (FTVector3 rotation)	{ mRotation = rotation; }
	void SetCurrentDirection (int dir);

public:
	Transform();
	Transform(Transform& origin);
	virtual ~Transform() {};

private:
	FTVector3	mWorldPosition;
	FTVector3	mScreenPosition;
	FTVector3	mLocalPosition;
	FTVector3	mScale;
	FTVector3	mRotation;

	int			mCurrentDirection;

public:
	static FTVector3 ConvertRadToDegree(FTVector3 radianRot);
	static FTVector3 ConvertDegreeToRad(FTVector3 degreeRot);

	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ofs);

#ifdef FOXTROT_EDITOR
public:
	void SaveProperties(nlohmann::ordered_json& out);
	void LoadProperties();
#endif // FOXTROT_EDITOR
};