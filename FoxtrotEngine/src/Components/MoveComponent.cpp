// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/MoveComponent.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Components/Rigidbody2DComponent.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Managers/CollisionManager.h"
#include "Physics/Physics2D.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

MoveComponent::MoveComponent(class Actor* owner, int drawOrder, int updateorder)
	: Component			(owner, drawOrder , updateorder)
	, mRigidbody		(nullptr)
	, mGroundFilter		(b2DefaultQueryFilter())
	, mForwardSpeed		(0.f)
	, mJumpForce		(0.f)
	, mAngularSpeed		(0.f)
	, mIsControllable	(Controllable::YES)
	, mHorizontalDir	(HorizontalDir::IDLE)
	, mVerticalDir		(VerticalDir::GROUND)
	, mIsGrounded		(false)
{
	mGroundFilter = CollisionManager::GetInstance()->GetQueryFilter(
		GetOwner()->GetActorGroup()
	);
}

void MoveComponent::Accelerate()
{
	b2Vec2 vel = b2Body_GetLinearVelocity(mRigidbody->GetBodyID());
	if(Math::Abs(vel.x) < Math::Abs(mHorizontalDir * mForwardSpeed))
		vel.x += mHorizontalDir * mForwardSpeed;
	else
		vel.x = mHorizontalDir * mForwardSpeed;
	b2Body_SetLinearVelocity(mRigidbody->GetBodyID(), vel);
	GetOwner()->GetTransform()->SetCurrentDirection(mHorizontalDir);
}

void MoveComponent::Jump()
{
	if (mIsGrounded)
	{
		b2Vec2 vel = b2Vec2_zero;
		vel.y = mVerticalDir * mJumpForce;
		b2Body_ApplyLinearImpulseToCenter(mRigidbody->GetBodyID(), vel, true);
	}
}

void MoveComponent::SetIsGrounded()
{
	b2RayCastInput rcInput;
	b2AABB aaBB = b2Body_ComputeAABB(mRigidbody->GetBodyID());
	b2Vec2 center = b2AABB_Center(aaBB);
	b2Vec2 extent = b2AABB_Extents(aaBB);
	b2Vec2 length = b2Vec2_zero;
	length.y = -0.5f - extent.y;
	b2RayResult result = b2World_CastRayClosest(
		Physics2D::GetInstance()->GetCurrentWorldID(),
		center,
		length,
		mGroundFilter
	);
	mIsGrounded = result.hit;
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
		SetIsGrounded();
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