#include "Components/GunFiringComponent.h"

#include "Managers/KeyInputManager.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Actors/Actor.h"
#include "Physics/Physics2D.h"

#include "FileSystem/ChunkLoader.h"

void GunFiringComponent::Update(float deltaTime)
{
	if (MOUSE_TAP(mTriggerKey) || MOUSE_HOLD(mTriggerKey))
	{
		if (mCurrentTick == 0)
			ShootGun();
	}

	if (mCurrentTick <= mDelay)
		mCurrentTick += deltaTime;
	else
		mCurrentTick = 0;
}

void GunFiringComponent::ShootGun()
{
	FTVector2 playerPos = GetOwner()->GetTransform()->GetWorldPosition();
	Physics2D::Raycast(
		playerPos,
		MOUSE_WORLDPOS,
		500.f,
		ActorGroup::ENEMY
	);
	LogString("Gun fired!");
}

GunFiringComponent::GunFiringComponent(Actor* owner, int drawOrder, int updateorder)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(MOUSE::MOUSE_LEFT)
	, mDelay(0.f)
	, mCurrentTick(0.f)
{
}

GunFiringComponent::GunFiringComponent(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(triggerKey)
	, mDelay(delay)
	, mCurrentTick(0.f)
{

}

GunFiringComponent::~GunFiringComponent()
{
}

void GunFiringComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}