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
		/// @brief Returns the local position used by this transform.
		/// @return Borrowed access to the local position.
		CORE_API const Math::FTVector3& GetLocalPosition() const;
		/// @brief Returns the local scale used by this transform.
		/// @return Borrowed access to the local scale.
		CORE_API const Math::FTVector3& GetLocalScale() const;
		/// @brief Returns the local rotation used by this transform.
		/// @return Borrowed access to the local rotation.
		CORE_API const Math::FTVector3& GetLocalRotation() const;

		// World Transformation
		/// @brief Returns the world position used by this transform.
		/// @return Borrowed access to the world position.
		CORE_API virtual const Math::FTVector3& GetWorldPosition() const;
		/// @brief Returns the world scale used by this transform.
		/// @return Borrowed access to the world scale.
		CORE_API virtual const Math::FTVector3& GetWorldScale() const;
		/// @brief Returns the world rotation used by this transform.
		/// @return Borrowed access to the world rotation.
		CORE_API const Math::FTVector3&		   GetWorldRotation() const;
		/// @brief Returns the matrix world used by this transform.
		/// @return Borrowed access to the matrix world.
		CORE_API virtual const Math::FTMatrix4& GetMatrixWorld() const;
		/// @brief Returns the rotation degree used by this transform.
		/// @return Current rotation degree.
		CORE_API const Math::FTVector3		   GetRotationDegree() const;
		/// @brief Returns the rightward used by this transform.
		/// @return Borrowed access to the rightward.
		CORE_API const Math::FTVector3&		   GetRightward() const;
		/// @brief Returns the steering used by this transform.
		/// @return Borrowed access to the steering.
		CORE_API virtual const Steering*		   GetSteering() const;

		/// @brief Tests the halting condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		CORE_API const bool IsHalting() const;

		// Local Transformation
		/// @brief Updates the local position used by subsequent operations.
		/// @param val Replacement local position.
		CORE_API void SetLocalPosition(const Math::FTVector3 val);
		/// @brief Updates the local scale used by subsequent operations.
		/// @param val Replacement local scale.
		CORE_API void SetLocalScale(const Math::FTVector3 val);
		/// @brief Updates the local rotation used by subsequent operations.
		/// @param val Replacement local rotation.
		CORE_API void SetLocalRotation(const Math::FTVector3 val);

		// World Transformation
		/// @brief Updates the world position used by subsequent operations.
		/// @param worldPos Replacement world position.
		CORE_API void		 SetWorldPosition(const Math::FTVector3 worldPos);
		/// @brief Updates the world scale used by subsequent operations.
		/// @param worldScale Replacement world scale.
		CORE_API virtual void SetWorldScale(const Math::FTVector3 worldScale);
		/// @brief Updates the world rotation used by subsequent operations.
		/// @param worldRot Replacement world rotation.
		CORE_API void		 SetWorldRotation(const Math::FTVector3 worldRot);

		/// @brief Updates the rightward used by subsequent operations.
		/// @param dir Replacement rightward.
		CORE_API void SetRightward(const Math::FTVector3 dir);
		/// @brief Updates the steering used by subsequent operations.
		/// @param steering Replacement steering.
		CORE_API void SetSteering(const Steering steering);

		/// @brief Updates the owner used by subsequent operations.
		/// @param actor Replacement owner.
		CORE_API void SetOwner(Common::IActor* actor);

		/// @brief Converts each Euler rotation component from radians to degrees.
		/// @param radianRot Euler rotation components in radians.
		/// @return Converts each Euler rotation component from radians to degrees.
		CORE_API static Math::FTVector3 ConvertRadToDegree(Math::FTVector3 radianRot);
		/// @brief Converts each Euler rotation component from degrees to radians.
		/// @param degreeRot Euler rotation components in degrees.
		/// @return Converts each Euler rotation component from degrees to radians.
		CORE_API static Math::FTVector3 ConvertDegreeToRad(Math::FTVector3 degreeRot);

	public:
		/// @brief Advances frame-dependent state using the current time step.
		CORE_API void Update();
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param target Target object or value sought by the operation.
		CORE_API void CloneTo(Transform* target);

	public:
		/// @brief Initializes local/world transform state and steering for its owner.
		/// @param owner Actor or object associated with the new instance.
		CORE_API Transform(Common::IActor* owner);
		/// @brief Initializes local/world transform state and steering for its owner.
		/// @param owner Actor or object associated with the new instance.
		/// @param origin Source instance whose state is copied.
		CORE_API Transform(Common::IActor* owner, Transform& origin);
		/// @brief Releases the resources managed by this instance during destruction.
		CORE_API virtual ~Transform();

	protected:
		/// @brief Returns the owner used by this transform.
		/// @return Borrowed access to the owner.
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
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		CORE_API void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ofs Input stream positioned at the serialized transform data.
		/// @note Advances the stream position and updates the destination state.
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
