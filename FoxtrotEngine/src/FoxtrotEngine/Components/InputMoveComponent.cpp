#include "FoxtrotEngine/Components/InputMoveComponent.h"

#include "FoxtrotEngine/Managers/KeyInputManager.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Components/Rigidbody2DComponent.h"

InputMoveComponent::InputMoveComponent(Actor* owner, int drawOrder, int updateOrder)
	: MoveComponent(owner, drawOrder, updateOrder)
	, mMaxForwardSpeed(0.f)
	, mMaxAngularSpeed(0.f)
	, mAccelerationForward(0.f)
	, mAccelerationAngular(0.f)
	, mBreakSpeed(0.f)
	, currentDir(0)
{}

void InputMoveComponent::ProcessInput(KeyInputManager* keyInputManager)
{
	Move();
	Rotate();
}

void InputMoveComponent::Update(float deltaTime)
{
	/*if (!Math::NearZero(GetAngularSpeed()))
	{
		float rot = GetOwner()->GetRotation();
		rot += GetAngularSpeed() * deltaTime;
		GetOwner()->SetRotation(rot);
	}*/
	MoveComponent::Update(deltaTime);
}

void InputMoveComponent::Move()
{
	////float forwardSpeed = GetForwardSpeed();
	//if (KEY_TAP(KEY::RIGHT))
	//{
	//	if (GetPrevMovingState() != MOVING_RIGHT)
	//		SetMovingState(MOVING_RIGHT);
	//}
	//if (KEY_TAP(KEY::LEFT))
	//{
	//	if (GetPrevMovingState() != MOVING_LEFT)
	//		SetMovingState(MOVING_LEFT);
	//}
	//if (KEY_TAP(KEY::UP))
	//{
	//	if (GetPrevMovingState() != MOVING_UP)
	//		SetMovingState(MOVING_UP);
	//}
	//if (KEY_TAP(KEY::DOWN))
	//{
	//	if (GetPrevMovingState() != MOVING_DOWN)
	//		SetMovingState(MOVING_DOWN);
	//}
}

void InputMoveComponent::LateUpdate(float deltaTime)
{
 //   State state = GetMovingState();
	//if (state == MoveComponent::State::IDLE)
	//{
	//}
	//if (state == MoveComponent::State::MOVING_LEFT)
	//{
	//	GetOwner()->GetTransform()->Translate(FTVector2(-20.f, 0.f) * deltaTime);
	//}
	//if (state == MoveComponent::State::MOVING_RIGHT)
	//{
	//	GetOwner()->GetTransform()->Translate(FTVector2(20.f, 0.f) * deltaTime);
	//}
	//if (state == MoveComponent::State::MOVING_UP)
	//{
	//	GetOwner()->GetComponent<class Rigidbody2DComponent>()->AddForce(FTVector2(0.f, -500.f));
	//}
	//if (state == MoveComponent::State::MOVING_DOWN)
	//{
	//	GetOwner()->GetTransform()->Translate(FTVector2(0.f, 20.f) * deltaTime);
	//}
}

void InputMoveComponent::Rotate()
{
	//float angularSpeed = GetAngularSpeed();
	//if (KEY_HOLD(KEY::LEFT))
	//{
	//	currentDir = 1;
	//	Accelerate(FTVector2(0.f, -100.f));
	//}
	//if (KEY_HOLD(KEY::RIGHT))
	//{
	//	currentDir = -1;
	//	Accelerate(FTVector2(0.f, 100.f));

	//}
	//SetAngularSpeed(angularSpeed);
}