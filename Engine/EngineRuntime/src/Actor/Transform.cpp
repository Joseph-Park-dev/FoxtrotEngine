// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor/Transform.h"

#include "Actor/IActor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "FTMath.h"
#include "Actor/Steering.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/FTString.h"

namespace Core
{
	/// @brief Returns the local position used by this transform.
	/// @return Borrowed access to the local position.
	const Math::FTVector3& Transform::GetLocalPosition() const { return mLocalPosition; }
	/// @brief Returns the local scale used by this transform.
	/// @return Borrowed access to the local scale.
	const Math::FTVector3& Transform::GetLocalScale() const { return mLocalScale; }
	/// @brief Returns the local rotation used by this transform.
	/// @return Borrowed access to the local rotation.
	const Math::FTVector3& Transform::GetLocalRotation() const { return mLocalRotation; }

	/// @brief Returns the world position used by this transform.
	/// @return Borrowed access to the world position.
	const Math::FTVector3& Transform::GetWorldPosition() const { return mWorldPosition; }
	/// @brief Returns the world scale used by this transform.
	/// @return Borrowed access to the world scale.
	const Math::FTVector3& Transform::GetWorldScale() const { return mWorldScale; }
	/// @brief Returns the world rotation used by this transform.
	/// @return Borrowed access to the world rotation.
	const Math::FTVector3& Transform::GetWorldRotation() const { return mWorldRotation; }
	/// @brief Returns the matrix world used by this transform.
	/// @return Borrowed access to the matrix world.
	const Math::FTMatrix4& Transform::GetMatrixWorld() const { return mMatrixWorld; }

	/// @brief Returns the rotation degree used by this transform.
	/// @return Current rotation degree.
	const Math::FTVector3 Transform::GetRotationDegree() const
	{
		return ConvertRadToDegree(mWorldRotation);
	}

	/// @brief Returns the rightward used by this transform.
	/// @return Borrowed access to the rightward.
	const Math::FTVector3& Transform::GetRightward() const
	{
		return mRightward;
	}

	/// @brief Returns the steering used by this transform.
	/// @return Borrowed access to the steering.
	const Steering* Transform::GetSteering() const
	{
		return mSteering;
	}

	/// @brief Tests the halting condition for the current object.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	const bool Transform::IsHalting() const
	{
		return mSteering->Linear == Math::FTVector3::Zero && !mSteering->JumpTriggered;
	}

	/// @brief Updates the local position used by subsequent operations.
	/// @param localPos Replacement local position.
	void Transform::SetLocalPosition(const Math::FTVector3 localPos)
	{
		mLocalPosition = localPos;
		Update();
	}

	/// @brief Updates the local scale used by subsequent operations.
	/// @param localScale Replacement local scale.
	void Transform::SetLocalScale(const Math::FTVector3 localScale)
	{
		mLocalScale = localScale;
		Update();
	}

	/// @brief Updates the local rotation used by subsequent operations.
	/// @param localRot Replacement local rotation.
	void Transform::SetLocalRotation(const Math::FTVector3 localRot)
	{
		mLocalRotation = localRot;
		Update();
	}

	/// @brief Updates the world position used by subsequent operations.
	/// @param worldPos Replacement world position.
	void Transform::SetWorldPosition(const Math::FTVector3 worldPos)
	{
		mWorldPosition		   = worldPos;
		Common::IActor* parent = mOwner->GetParent();
		if (!parent)
			SetLocalPosition(worldPos);
	}

	/// @brief Updates the world scale used by subsequent operations.
	/// @param worldScale Replacement world scale.
	void Transform::SetWorldScale(const Math::FTVector3 worldScale)
	{
		mWorldScale			   = worldScale;
		Common::IActor* parent = mOwner->GetParent();
		if (!parent)
			SetLocalScale(worldScale);
	}

	/// @brief Updates the world rotation used by subsequent operations.
	/// @param worldRot Replacement world rotation.
	void Transform::SetWorldRotation(const Math::FTVector3 worldRot)
	{
		mWorldRotation		   = worldRot;
		Common::IActor* parent = mOwner->GetParent();
		if (!parent)
			SetLocalRotation(worldRot);
	}

	/// @brief Updates the rightward used by subsequent operations.
	/// @param dir Replacement rightward.
	void Transform::SetRightward(Math::FTVector3 dir)
	{
		assert(mRightward != Math::FTVector3::Zero);
		mRightward = dir;
	}

	/// @brief Updates the steering used by subsequent operations.
	/// @param steering Replacement steering.
	void Transform::SetSteering(Steering steering)
	{
		mSteering->Angular		 = steering.Angular;
		mSteering->Linear		 = steering.Linear;
		mSteering->JumpTriggered = steering.JumpTriggered;
	}

	/// @brief Updates the owner used by subsequent operations.
	/// @param actor Replacement owner.
	void Transform::SetOwner(Common::IActor* actor)
	{
		mOwner = actor;
	}

	/// @brief Initializes local/world transform state and steering for its owner.
	/// @param owner Actor or object associated with the new instance.
	/// @note Initializes the :Transform base or delegates to its constructor.
	Transform::Transform(Common::IActor* owner)
		: mLocalPosition(Math::FTVector3::Zero)
		, mLocalScale(Math::FTVector3(1.f, 1.0f, 1.0f))
		, mLocalRotation(Math::FTVector3::Zero)
		, mMatrixLocal(Math::FTMatrix4::Identity)

		, mWorldPosition(Math::FTVector3::Zero)
		, mWorldScale(Math::FTVector3(1.0f, 1.0f, 1.0f))
		, mWorldRotation(Math::FTVector3(0.0f, 0.0f, 0.0f))
		, mMatrixWorld(Math::FTMatrix4::Identity)

		, mRightward(Math::FTVector3(1.0f, 0.0f, 0.0f))
		, mSteering(Steering::CreateEmptySteering())

		, mOwner(owner)
	{
	}

	/// @brief Initializes local/world transform state and steering for its owner.
	/// @param owner Actor or object associated with the new instance.
	/// @param origin Source instance whose state is copied.
	/// @note Initializes the :Transform base or delegates to its constructor.
	Transform::Transform(Common::IActor* owner, Transform& origin)
		: mLocalPosition(origin.mLocalPosition)
		, mLocalScale(origin.mLocalScale)
		, mLocalRotation(origin.mLocalRotation)
		, mMatrixLocal(origin.mMatrixLocal)

		, mWorldPosition(origin.mWorldPosition)
		, mWorldScale(origin.mWorldScale)
		, mWorldRotation(origin.mWorldRotation)
		, mMatrixWorld(origin.mMatrixWorld)

		, mRightward(origin.mRightward)
		, mSteering(Steering::CreateEmptySteering())

		, mOwner(owner)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	Transform::~Transform()
	{
		if (mSteering)
		{
			delete mSteering;
			mSteering = nullptr;
		}
	}

	/// @brief Returns the owner used by this transform.
	/// @return Borrowed access to the owner.
	const Common::IActor* Transform::GetOwner() const
	{
		return mOwner;
	}

	/// @brief Converts each Euler rotation component from radians to degrees.
	/// @param radianRot Euler rotation components in radians.
	/// @return Converts each Euler rotation component from radians to degrees.
	Math::FTVector3 Transform::ConvertRadToDegree(Math::FTVector3 radianRot)
	{
		float x = Math::ToDegrees(radianRot.x);
		float y = Math::ToDegrees(radianRot.y);
		float z = Math::ToDegrees(radianRot.z);
		return Math::FTVector3(x, y, z);
	}

	/// @brief Converts each Euler rotation component from degrees to radians.
	/// @param degreeRot Euler rotation components in degrees.
	/// @return Converts each Euler rotation component from degrees to radians.
	Math::FTVector3 Transform::ConvertDegreeToRad(Math::FTVector3 degreeRot)
	{
		float x = Math::ToRadians(degreeRot.x);
		float y = Math::ToRadians(degreeRot.y);
		float z = Math::ToRadians(degreeRot.z);
		return Math::FTVector3(x, y, z);
	}

	/// @brief Advances frame-dependent state using the current time step.
	void Transform::Update()
	{
		Math::FTMatrix4 matScale	= Math::FTMatrix4::CreateScale(mLocalScale);
		Math::FTMatrix4 matRotation = Math::FTMatrix4::CreateRotationX(mLocalRotation.x) *
			Math::FTMatrix4::CreateRotationY(mLocalRotation.y) *
			Math::FTMatrix4::CreateRotationZ(mLocalRotation.z);
		Math::FTMatrix4 matTranslation = Math::FTMatrix4::CreateTranslation(mLocalPosition);

		mMatrixLocal = matScale * matRotation * matTranslation;

		Common::IActor* parent = mOwner->GetParent();
		if (parent)
			mMatrixWorld = mMatrixLocal * parent->GetTransform()->mMatrixWorld;
		else
			mMatrixWorld = mMatrixLocal;

		Common::FTDS::DynamicArray<Common::IActor*>* childActors = mOwner->GetChildActors();
		if (0 < childActors->GetSize())
		{
			for (auto child = childActors->Begin(); child != childActors->End(); ++child)
				(*child)->GetTransform()->Update();
		}
		// Math::FTVector3::DecomposeMatrix(mWorldScale, mWorldRotation, mWorldPosition, mMatrixWorld);
	}

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param target Target object or value sought by the operation.
	void Transform::CloneTo(Transform* target)
	{
		target->mOwner = mOwner;

		if (mOwner->GetParent())
		{
			target->SetLocalPosition(mLocalPosition);
			target->SetLocalScale(mLocalScale);
			target->SetLocalRotation(mLocalRotation);
		}
		else
		{
			target->SetWorldPosition(mWorldPosition);
			target->SetWorldScale(mWorldScale);
			target->SetWorldRotation(mWorldRotation);
		}
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void Transform::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::TRANSFORM);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_ROTATION, mWorldRotation);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_SCALE, mWorldScale);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::WORLD_POS, mWorldPosition);

		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_ROTATION, mLocalRotation);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_SCALE, mLocalScale);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::LOCAL_POS, mLocalPosition);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::TRANSFORM);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void Transform::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::TRANSFORM);
		Common::FileIOHelper::LoadVector3(ifs, mLocalPosition);
		Common::FileIOHelper::LoadVector3(ifs, mLocalScale);
		Common::FileIOHelper::LoadVector3(ifs, mLocalRotation);

		Common::FileIOHelper::LoadVector3(ifs, mWorldPosition);
		Common::FileIOHelper::LoadVector3(ifs, mWorldScale);
		Common::FileIOHelper::LoadVector3(ifs, mWorldRotation);

		SetWorldPosition(mWorldPosition);
		SetWorldScale(mWorldScale);
		SetWorldRotation(mWorldRotation);
	}

	// const MemberDesc MemberLayout[] = {
	//	MakeMemberFuncDesc("GetWorldScale", &Transform::GetWorldScale),
	// };

	// const ObjectLayout Layout = {
	//	"Transform",
	//	sizeof(Transform),
	//	MemberLayout,
	//	sizeof(MemberLayout) / sizeof(MemberLayout[0])
	// };

	// CORE_API const ObjectLayout* Plugin_GetLayout() { return &Layout; }

	// static const ObjectLayout g_layout = {
	//	"Player",
	//	sizeof(Transform),
	//	g_memberLayout,
	//	sizeof(g_memberLayout) / sizeof(g_memberLayout[0])
	// };
} // namespace Core
