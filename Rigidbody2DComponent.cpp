#include "Rigidbody2DComponent.h"
#include "Physics2D.h"
#include "Actor.h"
#include "Math.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "EditorCamera2D.h"

Rigidbody2DComponent::Rigidbody2DComponent(class Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mMass(1.f)
	, mForce(FTVector2::Zero)
	, mAcceleration(FTVector2::Zero)
	, mAccelerationA(FTVector2::Zero)
	, mVelocity(FTVector2::Zero)
	, mMaxVelocity(FTVector2(200.f, 600.f))
	, mFrictionCoeff(0.1f)
	, mIsGrounded(false)
	, mIsBlockedUp(false)
{}

void Rigidbody2DComponent::CloneTo(Actor* actor)
{
	Rigidbody2DComponent* newComp = new Rigidbody2DComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mMass = mMass;
	newComp->mMaxVelocity = mMaxVelocity;
}

void Rigidbody2DComponent::AddForce(FTVector2 force)
{
	mForce += force;
}

void Rigidbody2DComponent::AddVelocity(FTVector2 velocity)
{
	mVelocity += velocity;
}

void Rigidbody2DComponent::LateUpdate(float deltaTime)
{
	UpdateLinearPosition(deltaTime);
	UpdateAcceleration(deltaTime);
	UpdateGravity(deltaTime);
	UpdateVelocity(deltaTime);
	ClearForceAndAccel();
}

void Rigidbody2DComponent::UpdateLinearPosition(float deltaTime)
{
	FTVector2 pos = GetOwner()->GetTransform()->GetWorldPosition();
	pos += mVelocity * deltaTime;
	GetOwner()->GetTransform()->SetWorldPosition(pos);
}

void Rigidbody2DComponent::UpdateAcceleration(float deltaTime)
{
	if (mForce.Length() != 0.f)
	{
		FTVector2 resultingAcc = mForce / mMass;
		mAcceleration += resultingAcc;
	}
	mAcceleration += mAccelerationA;
}

void Rigidbody2DComponent::UpdateGravity(float deltaTime)
{
	if (!mIsGrounded)
		mAccelerationA = FTVector2(0.f, 600.f);
	else
	{
		mAccelerationA = FTVector2::Zero;
		mVelocity = FTVector2(mVelocity.x, 0.f);
	}
	if (mIsBlockedUp)
	{
		mAccelerationA = FTVector2(0.f, 600.f);
		mVelocity = FTVector2(mVelocity.x, 0.f);
	}
}

void Rigidbody2DComponent::UpdateVelocity(float deltaTime)
{
	mVelocity += mAcceleration * deltaTime;

	// Applying friction to the object
	if (mIsGrounded && mVelocity.x != 0)
	{
		int frictionDir = -(mVelocity.x / Math::Abs(mVelocity.x));
		if (Math::Abs(mVelocity.x) < 0.1f)
			mVelocity.x = 0.f;
		else
			mVelocity *= powf(mFrictionCoeff, deltaTime);
	}

	if (Math::Abs(mMaxVelocity.x) <= Math::Abs(mVelocity.x))
	{
		mVelocity.x = (mVelocity.x / Math::Abs(mVelocity.x))
			* Math::Abs(mMaxVelocity.x);
	}
	if (Math::Abs(mMaxVelocity.y) <= Math::Abs(mVelocity.y))
	{
		mVelocity.y = (mVelocity.y / Math::Abs(mVelocity.y))
			* Math::Abs(mMaxVelocity.y);
	}
}

void Rigidbody2DComponent::ClearForceAndAccel()
{
	mForce = FTVector2::Zero;
	mAcceleration = FTVector2::Zero;
}

void Rigidbody2DComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::AddFloat(ofs, mMass);
	FileIOHelper::AddFloat(ofs, mFrictionCoeff);
	FileIOHelper::AddVector2(ofs, mMaxVelocity);
}

void Rigidbody2DComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	FileIOHelper::LoadFloat(ifs, mMass);
	FileIOHelper::LoadFloat(ifs, mFrictionCoeff);
	FileIOHelper::LoadVector2(ifs, mMaxVelocity);
}