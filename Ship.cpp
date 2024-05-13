#include "Ship.h"
#include "FTCore.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "UIActor.h"
#include "KeyInputManager.h"
#include "Physics2D.h"
#include "Camera2D.h"
#include "Transform.h"

#include "AnimSpriteComponent.h"
#include "AnimatorComponent.h"
#include "InputMoveComponent.h"
#include "ColliderComponent.h"
#include "Rigidbody2DComponent.h"
#include "SpriteComponent.h"
#include "GunFiringComponent.h"

Ship::Ship(Scene* scene)
	: Actor(scene)
	, mCurrentState(PLAYER_STATE::IDLE)
	, mPreviousState(PLAYER_STATE::IDLE)
	, rb(nullptr)
	, ac(nullptr)
	, gun(nullptr)
	, mDirection(1)
{
	InitializeActor();
}

void Ship::InitializeActor()
{
}

void Ship::UpdateActor(float deltaTime)
{
	//UpdateState();
	UpdateAnimation();
	mPreviousState = mCurrentState;
}

void Ship::UpdateAnimation()
{
	if (mCurrentState == mPreviousState)
		return;

	switch (mCurrentState)
	{
	case PLAYER_STATE::WALKING_LEFT:
		ac->Play(L"WalkingLeft", true);
	}
}
