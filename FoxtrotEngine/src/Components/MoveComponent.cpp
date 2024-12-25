#include "Components/MoveComponent.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Components/Rigidbody2DComponent.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR


MoveComponent::MoveComponent(class Actor* owner, int drawOrder, int updateorder)
	: Component			(owner, drawOrder , updateorder)
	, mRigidbody		(nullptr)
	, mForwardSpeed		(0.f)
	, mJumpForce		(0.f)
	, mAngularSpeed		(0.f)
	, mIsControllable	(Controllable::YES)
	, mHorizontalDir	(HorizontalDir::IDLE)
	, mVerticalDir		(VerticalDir::GROUND)
{}

void MoveComponent::Accelerate()
{
	b2Vec2 vel = b2Body_GetLinearVelocity(mRigidbody->GetBodyID());
	if(Math::Abs(vel.x) < Math::Abs(mHorizontalDir * mForwardSpeed))
		vel.x += mHorizontalDir * mForwardSpeed;
	else
		vel.x = mHorizontalDir * mForwardSpeed;
	b2Body_SetLinearVelocity(mRigidbody->GetBodyID(), vel);
}

void MoveComponent::Jump()
{
	b2Vec2 vel = b2Vec2_zero;
	vel.y = mVerticalDir * mJumpForce;
	b2Body_ApplyLinearImpulseToCenter(mRigidbody->GetBodyID(), vel, true);
}

void MoveComponent::Initialize(FTCore* coreInstance)
{
	mRigidbody = GetOwner()->GetComponent<Rigidbody2DComponent>();
	assert(mRigidbody);  // Rigidbody is crucial for this compoenent to work.
	if (mRigidbody)
		Component::Initialize(coreInstance);
}

void MoveComponent::Update(float deltaTime)
{
	if (mIsControllable)
	{
		if (mHorizontalDir != HorizontalDir::IDLE)
			Accelerate();
		if (mVerticalDir != VerticalDir::GROUND)
			Jump();
	}
}

void MoveComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::FORWARD_SPEED,	mForwardSpeed);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::JUMP_FORCE,		mJumpForce);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::ANGULAR_SPEED,	mAngularSpeed);
}

void MoveComponent::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadFloat(ifs, mForwardSpeed);
	FileIOHelper::LoadFloat(ifs, mJumpForce);
	FileIOHelper::LoadFloat(ifs, mAngularSpeed);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void MoveComponent::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::FORWARD_SPEED, &mForwardSpeed, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::JUMP_FORCE, &mJumpForce, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::ANGULAR_SPEED, &mAngularSpeed, FLOATMOD_SPEED);
}

void MoveComponent::CloneTo(Actor* actor)
{
	MoveComponent* newComp	= DBG_NEW MoveComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mForwardSpeed	= this->mForwardSpeed;
	newComp->mJumpForce		= this->mJumpForce;
	newComp->mAngularSpeed	= this->mAngularSpeed;
}
#endif // FOXTROT_EDITOR
