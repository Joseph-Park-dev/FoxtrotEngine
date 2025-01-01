// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/ActorBehaviors/Bullet/BulletBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/EventFunctions.h"

void BulletBehavior::SetStartPosition(FTVector2 pos)
{
	mStartPosition = pos;
}

void BulletBehavior::SetImpulseQuantity(FTVector2 impulse)
{
	mImpulseQuantity = impulse * FTVector2(1.0f, -1.0f);
}

void BulletBehavior::SetDirection(FTVector2 direction)
{
	mDirection = direction;
}

void BulletBehavior::SetSpeed(float speed)
{
	mSpeed = speed;
}

void BulletBehavior::Initialize(FTCore* coreInstance)
{
	mRigidbody = GetOwner()->GetComponent<Rigidbody2DComponent>();
	Component::Initialize(coreInstance);
}

void BulletBehavior::Setup()
{
	GetOwner()->GetTransform()->SetWorldPosition(mStartPosition);
	b2Body_SetTransform(
		mRigidbody->GetBodyID(),
		mStartPosition.GetB2Vec2(),
		b2MakeRot(GetOwner()->GetTransform()->GetRotation().z)
	);
	SetImpulseQuantity(mDirection * mSpeed);
	Thrust();
	Component::Setup();
}

void BulletBehavior::Update(float deltaTime)
{
	if (mDuration <= 0)
		Destroy(this->GetOwner());
	mDuration -= deltaTime;
}

void BulletBehavior::LateUpdate(float deltaTime)
{
	// Updating bullet position 
	GetOwner()->GetTransform()->SetWorldPosition(
		b2Body_GetPosition(mRigidbody->GetBodyID())
	);

	// Updating bullet rotation
	b2Rot  rotation = b2Body_GetRotation(mRigidbody->GetBodyID());
	float rotZ = b2Rot_GetAngle(rotation);
	FTVector3 prevRot = GetOwner()->GetTransform()->GetRotation();
	FTVector3 updatedRot = FTVector3(prevRot.x, prevRot.y, -rotZ);
	GetOwner()->GetTransform()->SetRotation(updatedRot);
}

BulletBehavior::BulletBehavior(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER)
	: Component(owner, drawOrder, updateOrder)
	, mRigidbody(nullptr)
	, mImpulseQuantity(FTVector2::Zero)
	, mDirection(FTVector2::Zero)
	, mStartPosition(FTVector2::Zero)
	, mSpeed(10.0f)
	, mDuration(3.0f)
{}

void BulletBehavior::Thrust()
{
	if (!mImpulseQuantity.IsZero())
	{
		b2Body_ApplyLinearImpulseToCenter(
			mRigidbody->GetBodyID(),
			mImpulseQuantity.GetB2Vec2(),
			true
		);
	}
}

#ifdef FOXTROT_EDITOR
void BulletBehavior::CloneTo(Actor* actor)
{
	BulletBehavior* newComp = DBG_NEW BulletBehavior(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mImpulseQuantity = this->mImpulseQuantity;
	newComp->mRigidbody = this->mRigidbody;
}
#endif