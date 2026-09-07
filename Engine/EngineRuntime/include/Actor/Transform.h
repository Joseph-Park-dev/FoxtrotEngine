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
#include "Plugin/CoreExports.h"
#include "Entity/Entity.h"

#include <iosfwd>

#include "FTMath.h"
#include "Plugin/CoreExports.h"

namespace Common
{
	class IActor;
} // namespace Common

namespace Core
{
	struct Steering;

	class Transform
	{
	public:
		//////////////////////////////
		///// Getter and Setters /////
		//////////////////////////////

		// Local Transformation
		CORE_API const Math::FTVector3& GetLocalPosition() const;
		CORE_API const Math::FTVector3& GetLocalScale() const;
		CORE_API const Math::FTVector3& GetLocalRotation() const;

		// World Transformation
		CORE_API virtual const Math::FTVector3& GetWorldPosition() const;
		CORE_API virtual const Math::FTVector3& GetWorldScale() const;
		CORE_API const Math::FTVector3&		   GetWorldRotation() const;
		CORE_API virtual const Math::FTMatrix4& GetMatrixWorld() const;
		CORE_API const Math::FTVector3		   GetRotationDegree() const;
		CORE_API const Math::FTVector3&		   GetRightward() const;
		CORE_API virtual const Steering*		   GetSteering() const;

		CORE_API const bool IsHalting() const;

		// Local Transformation
		CORE_API void SetLocalPosition(const Math::FTVector3 val);
		CORE_API void SetLocalScale(const Math::FTVector3 val);
		CORE_API void SetLocalRotation(const Math::FTVector3 val);

		// World Transformation
		CORE_API void		 SetWorldPosition(const Math::FTVector3 worldPos);
		CORE_API virtual void SetWorldScale(const Math::FTVector3 worldScale);
		CORE_API void		 SetWorldRotation(const Math::FTVector3 worldRot);

		CORE_API void SetRightward(const Math::FTVector3 dir);
		CORE_API void SetSteering(const Steering steering);

		CORE_API void SetOwner(Common::IActor* actor);

		CORE_API static Math::FTVector3 ConvertRadToDegree(Math::FTVector3 radianRot);
		CORE_API static Math::FTVector3 ConvertDegreeToRad(Math::FTVector3 degreeRot);

	public:
		CORE_API void Update();
		CORE_API void CloneTo(Transform* target);

	public:
		CORE_API Transform(Common::IActor* owner);
		CORE_API Transform(Common::IActor* owner, Transform& origin);
		CORE_API virtual ~Transform();

	protected:
		CORE_API const Common::IActor* GetOwner() const;

	private:
		// Local Transformation.
		Math::FTVector3 mLocalPosition;
		Math::FTVector3 mLocalScale;
		Math::FTVector3 mLocalRotation;
		Math::FTMatrix4 mMatrixLocal;

		// World Transformation.
		Math::FTVector3 mWorldPosition;
		Math::FTVector3 mWorldScale;
		Math::FTVector3 mWorldRotation;
		Math::FTMatrix4 mMatrixWorld;

		Math::FTVector3 mRightward; // A local rightward direction
		Steering*		mSteering;

		Common::IActor* mOwner;

	public:
		CORE_API void SaveProperties(std::ofstream& ofs);
		CORE_API void LoadProperties(std::ifstream& ofs);
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
} // namespace Core