#include "Components/ActorBehaviors/Bullets/BulletBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/EventFunctions.h"

void BulletBehavior::SetImpulseQuantity(FTVector2 impulse)
{
	mImpulseQuantity = impulse * FTVector2(1.0f, -1.0f);
}

void BulletBehavior::Initialize(FTCore* coreInstance)
{
	mRigidbody = GetOwner()->GetComponent<Rigidbody2DComponent>();
	if(mRigidbody)
		Component::Initialize(coreInstance);
}

void BulletBehavior::Update(float deltaTime)
{
	if (mDuration <= 0)
		Destroy(this->GetOwner());
	mDuration -= deltaTime;
}

void BulletBehavior::LateUpdate(float deltaTime)
{
	//GetOwner()->GetTransform()->SetWorldPosition(
	//	b2Body_GetPosition(mRigidbody->GetBodyID())
	//);

	//b2Rot  rotation = b2Body_GetRotation(mRigidbody->GetBodyID());
	//float rotZ = b2Rot_GetAngle(rotation);
	//FTVector3 prevRot = GetOwner()->GetTransform()->GetRotation();
	//FTVector3 updatedRot = FTVector3(prevRot.x, prevRot.y, -rotZ);
	//GetOwner()->GetTransform()->SetRotation(updatedRot);
	//LogVector3("Rotation", updatedRot);
}

BulletBehavior::BulletBehavior(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER)
	: Component(owner, drawOrder, updateOrder)
	, mRigidbody(nullptr)
	, mImpulseQuantity(FTVector2::Zero)
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

void BulletBehavior::CloneTo(Actor* actor)
{
	BulletBehavior* newComp = DBG_NEW BulletBehavior(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mImpulseQuantity = this->mImpulseQuantity;
	newComp->mRigidbody = this->mRigidbody;
}
