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

class Actor;
struct Vertex;
class Camera;
class FTMatrix4;
struct Steering;

class Transform
{
public:
	//////////////////////////////
	///// Getter and Setters /////
	//////////////////////////////

	// Local Transformation
	const FTVector3& GetLocalPosition() const;
	const FTVector3& GetLocalScale() const;
	const FTVector3& GetLocalRotation() const;

	// World Transformation
	const FTVector3& GetWorldPosition() const;
	const FTVector3& GetWorldScale() const;
	const FTVector3& GetWorldRotation() const;
	const FTMatrix4& GetMatrixWorld() const;
	const FTVector3	 GetRotationDegree() const;
	const FTVector3& GetRightward() const;
	const Steering*	 GetSteering() const;

	const bool IsHalting() const;

	// Local Transformation
	void SetLocalPosition(const FTVector3 val);
	void SetLocalScale(const FTVector3 val);
	void SetLocalRotation(const FTVector3 val);

	// World Transformation
	void SetWorldPosition(const FTVector3 worldPos);
	void SetWorldScale(const FTVector3 worldScale);
	void SetWorldRotation(const FTVector3 worldRot);

	void SetRightward(const FTVector3 dir);
	void SetSteering(const Steering steering);

	void SetOwner(Actor* actor);

	static FTVector3 ConvertRadToDegree(FTVector3 radianRot);
	static FTVector3 ConvertDegreeToRad(FTVector3 degreeRot);

public:
	void Update();
	void CloneTo(Transform* target);

public:
	Transform(Actor* owner);
	Transform(Actor* owner, Transform& origin);
	virtual ~Transform();

private:
	// Local Transformation.
	FTVector3 mLocalPosition;
	FTVector3 mLocalScale;
	FTVector3 mLocalRotation;
	FTMatrix4 mMatrixLocal;

	// World Transformation.
	FTVector3 mWorldPosition;
	FTVector3 mWorldScale;
	FTVector3 mWorldRotation;
	FTMatrix4 mMatrixWorld;

	FTVector3 mRightward; // A local rightward direction
	Steering* mSteering;

	Actor* mOwner;

public:
	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ofs);

#ifdef FOXTROT_EDITOR
	void UpdateUI();
#endif
};

namespace ChunkKey
{
	constexpr const char* LOCAL_POS		 = "Local Position";
	constexpr const char* LOCAL_SCALE	 = "Local Scale";
	constexpr const char* LOCAL_ROTATION = "Local Rotation";

	constexpr const char* WORLD_POS		 = "World Position";
	constexpr const char* WORLD_SCALE	 = "World Scale";
	constexpr const char* WORLD_ROTATION = "World Rotation";
} // namespace ChunkKey