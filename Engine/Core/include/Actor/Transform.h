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
#include "Entity/Entity.h"

#include <iosfwd>

#include "FTMath.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	class Actor;
	struct Steering;

	class CORE_API Transform :
		Core::Entity
	{
	public:
		//////////////////////////////
		///// Getter and Setters /////
		//////////////////////////////

		// Local Transformation
		const Math::FTVector3& GetLocalPosition() const;
		const Math::FTVector3& GetLocalScale() const;
		const Math::FTVector3& GetLocalRotation() const;

		// World Transformation
		virtual const Math::FTVector3& GetWorldPosition() const;
		virtual const Math::FTVector3& GetWorldScale() const;
		const Math::FTVector3&		   GetWorldRotation() const;
		virtual const Math::FTMatrix4& GetMatrixWorld() const;
		const Math::FTVector3		   GetRotationDegree() const;
		const Math::FTVector3&		   GetRightward() const;
		virtual const Steering*		   GetSteering() const;

		const bool IsHalting() const;

		// Local Transformation
		void SetLocalPosition(const Math::FTVector3 val);
		void SetLocalScale(const Math::FTVector3 val);
		void SetLocalRotation(const Math::FTVector3 val);

		// World Transformation
		void		 SetWorldPosition(const Math::FTVector3 worldPos);
		virtual void SetWorldScale(const Math::FTVector3 worldScale);
		void		 SetWorldRotation(const Math::FTVector3 worldRot);

		void SetRightward(const Math::FTVector3 dir);
		void SetSteering(const Steering steering);

		void SetOwner(Actor* actor);

		static Math::FTVector3 ConvertRadToDegree(Math::FTVector3 radianRot);
		static Math::FTVector3 ConvertDegreeToRad(Math::FTVector3 degreeRot);

	public:
		void Update();
		void CloneTo(Transform* target);

	public:
		Transform(Actor* owner);
		Transform(Actor* owner, Transform& origin);
		virtual ~Transform();

	protected:
		const Actor* GetOwner() const;

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

		Actor* mOwner;

	public:
		void SaveProperties(std::ofstream& ofs);
		void LoadProperties(std::ifstream& ofs);
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