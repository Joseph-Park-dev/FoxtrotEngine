#include "Components/InputMoveComponent.h"

#include "Managers/KeyInputManager.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Components/Rigidbody2DComponent.h"

InputMoveComponent::InputMoveComponent(Actor* owner, int drawOrder, int updateOrder)
	: MoveComponent(owner, drawOrder, updateOrder)
	, mMaxForwardSpeed(1.7f)
	, mMaxAngularSpeed(0.f)
	, mAccelerationForward(0.f)
	, mAccelerationAngular(0.f)
	, mBreakSpeed(0.f)
	, currentDir(0)
{}

void InputMoveComponent::Initialize(FTCore* coreInstance)
{}

void InputMoveComponent::ProcessInput(KeyInputManager* keyInputManager)
{
	if (KEY_HOLD(KEY::A)) {
		mMoveState = MoveState::LeftMove;
	}
	if (KEY_HOLD(KEY::D)) {
		mMoveState = MoveState::RightMove;
	}
	if (KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) {
		mMoveState = MoveState::Idle;
	}
}

void InputMoveComponent::Update(float deltaTime)
{
	int dir = 0;
	if (mMoveState == MoveState::LeftMove) {
		dir = -1;
	}
	else if (mMoveState == MoveState::RightMove) {
		dir = 1;
	}
	GetOwner()->GetTransform()->SetCurrentDirection(dir);
	FTVector3 pos = GetOwner()->GetTransform()->GetWorldPosition();
	GetOwner()->GetTransform()->SetWorldPosition(
		pos + FTVector3(mMaxForwardSpeed,0.f,0.f) * deltaTime * dir
	);
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

#ifdef FOXTROT_EDITOR
// Editor code comes here.

#endif // FOXTROT_EDITOR