// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Actor's Transform data that defines its position, rotation, scale
/// and its current direction.
/// </summary>

#pragma once
#include "Math/FTMath.h"

#include "Components/CharacterAI/Steering.h"

class Transform
{
public:
	const FTVector3  GetWorldPosition()		const;
	const FTVector3  GetLocalPosition()		const;
	const FTVector3  GetScale()				const;
	const FTVector3	 GetRotation()			const;
	const FTVector3  GetRotationDegree()	const;
	const FTVector3	 GetRightward()			const;

	void SetWorldPosition	(const FTVector3 pos);
	void SetLocalPosition	(const FTVector3 pos);
	void SetScale		    (const FTVector3 scale);
	void SetRotation		(const FTVector3 rotation);
	void SetRightward		(const FTVector3 dir);
	void SetSteering		(const Steering steering);

	static FTVector3 ConvertRadToDegree(FTVector3 radianRot);
	static FTVector3 ConvertDegreeToRad(FTVector3 degreeRot);

public:
			 Transform	();
			 Transform	(Transform& origin);
	virtual ~Transform	();

private:
	Steering*	mSteering;

	FTVector3	mWorldPosition;
	FTVector3	mLocalPosition;
	FTVector3	mScale;
	FTVector3	mRotation;
	FTVector3	mRightward;	// A local rightward direction

public:
	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ofs);
};