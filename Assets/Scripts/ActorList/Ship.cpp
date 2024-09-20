#include "Scripts/ActorList/Ship.h"

#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/Scenes/Scene.h"
#include "FoxtrotEngine/Actors/UIs/UIActor.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Physics/Physics2D.h"


#include "FoxtrotEngine/Components/SpriteAnimComponent.h"
#include "FoxtrotEngine/Components/AnimatorComponent.h"
#include "FoxtrotEngine/Components/InputMoveComponent.h"
#include "FoxtrotEngine/Components/ColliderComponent.h"
#include "FoxtrotEngine/Components/Rigidbody2DComponent.h"
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"
#include "FoxtrotEngine/Components/GunFiringComponent.h"

Ship::Ship(Scene* scene)
	: Actor(scene)
	, mCurrentState(PLAYER_STATE::IDLE)
	, mPreviousState(PLAYER_STATE::IDLE)
	, rb(nullptr)
	, ac(nullptr)
	, gun(nullptr)
	, mDirection(1)
{
	Initialize();
}

void Ship::Initialize()
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
		ac->Play("WalkingLeft", true);
	}
}
