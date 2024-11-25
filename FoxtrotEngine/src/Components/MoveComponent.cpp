#include "Components/MoveComponent.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Components/Rigidbody2DComponent.h"
#include "Actors/Transform.h"

MoveComponent::MoveComponent(class Actor* owner, int drawOrder, int updateorder)
	:Component(owner, drawOrder , updateorder)
	, mAngularSpeed(0.f)
	, mForwardSpeed(0.f)
	, mMovingState(State::IDLE)
{
	//mRigidbody = GetOwner()->GetComponent<Rigidbody2DComponent>();
}

void MoveComponent::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}

void MoveComponent::Update(float deltaTime)
{
	FTVector3 pos = GetOwner()->GetTransform()->GetWorldPosition();
	GetOwner()->GetTransform()->SetWorldPosition(pos + mVelocity * deltaTime);
}

void MoveComponent::Accelerate(float& currentSpeed, float accel, float maxSpeed, int forwardDir)
{
	if (Math::Abs(currentSpeed) <= maxSpeed)
	{
		currentSpeed += accel * forwardDir;
	}
	else
	{
		currentSpeed = maxSpeed * forwardDir;
	}
}

void MoveComponent::Accelerate(FTVector2 velocity)
{
	//GetOwner()->GetComponent<class Rigidbody2DComponent>()->AddForce(velocity);
}

void MoveComponent::Deccelerate(float& currentSpeed, int& currentDir, float breakSpeed)
{
	if (Math::Abs(currentSpeed) > 0)
	{
		currentSpeed -= breakSpeed * currentDir;
	}
	else
	{
		currentSpeed = 0.f;
		currentDir = 0;
	}
}
