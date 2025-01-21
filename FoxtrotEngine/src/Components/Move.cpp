// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/Move.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Components/Rigidbody2D.h"
#include "Components/CharacterAI/Steering.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Managers/CollisionManager.h"
#include "Physics/Physics2D.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

Move::Move(Actor* owner, int updateorder)
	: Component(owner , updateorder)
	, mRigidbody		(nullptr)
	, mGroundFilter		(b2DefaultQueryFilter())
	, mForwardSpeed		(0.f)
	, mJumpForce		(0.f)
	, mAngularSpeed		(0.f)
	, mIsControllable	(Controllable::YES)
	, mIsGrounded		(false)
{
	mGroundFilter = CollisionManager::GetInstance()->GetQueryFilter(
		GetOwner()->GetActorGroup()
	);
}

void Move::Accelerate(b2Vec2 currVel, const Steering* steering)
{
	if (0 < Math::Abs(steering->Linear.x))
	{
		if (Math::Abs(currVel.x) < mForwardSpeed)
			currVel.x += (mForwardSpeed * steering->Linear.x);
		else
			currVel.x = (mForwardSpeed * steering->Linear.x);
	}

	if (0 < Math::Abs(steering->Linear.y))
	{
		if (Math::Abs(currVel.y) < mForwardSpeed)
			currVel.y += currVel.y + (mForwardSpeed * steering->Linear.y);
		else
			currVel.y = (mForwardSpeed * steering->Linear.y);
	}
	b2Body_SetLinearVelocity(mRigidbody->GetBodyID(), currVel);
}

void Move::Jump(b2Vec2 currVel, const Steering* steering)
{
	if (mIsGrounded)
	{
		b2Vec2 vel = b2Vec2_zero;
		vel.y = currVel.y + mJumpForce;
		b2Body_ApplyLinearImpulseToCenter(mRigidbody->GetBodyID(), vel, true);
	}
}

void Move::SetIsGrounded()
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

void Move::Initialize(FTCore* coreInstance)
{
	mRigidbody = GetOwner()->GetComponent<Rigidbody2D>();
	if (mRigidbody)
		Component::Initialize(coreInstance);
}

void Move::LateUpdate(float deltaTime)
{
	if (mIsControllable)
	{
		const Steering* steering = GetOwner()->GetTransform()->GetSteering();
		if (*steering != Steering::Halt())
		{
			b2Vec2 vel = b2Body_GetLinearVelocity(mRigidbody->GetBodyID());
			Accelerate(vel, steering);
			if (steering->JumpTriggered)
				Jump(vel, steering);
		}
		SetIsGrounded();
		GetOwner()->GetTransform()->SetSteering(Steering::Halt());
	}
}

void Move::CloneTo(Actor* actor)
{
	Move* newComp = DBG_NEW Move(actor, GetUpdateOrder());
	newComp->mForwardSpeed = this->mForwardSpeed;
	newComp->mJumpForce = this->mJumpForce;
	newComp->mAngularSpeed = this->mAngularSpeed;
}

void Move::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::FORWARD_SPEED,	mForwardSpeed);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::JUMP_FORCE,		mJumpForce);
	FileIOHelper::SaveFloat(ofs, ChunkKeys::ANGULAR_SPEED,	mAngularSpeed);
}

void Move::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadFloat(ifs, mForwardSpeed);
	FileIOHelper::LoadFloat(ifs, mJumpForce);
	FileIOHelper::LoadFloat(ifs, mAngularSpeed);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void Move::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::FORWARD_SPEED, &mForwardSpeed, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::JUMP_FORCE, &mJumpForce, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKeys::ANGULAR_SPEED, &mAngularSpeed, FLOATMOD_SPEED);
}
#endif // FOXTROT_EDITOR