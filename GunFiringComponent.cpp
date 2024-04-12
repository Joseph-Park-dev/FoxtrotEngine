#include "GunFiringComponent.h"
#include "KeyInputManager.h"
#include "Transform.h"
#include "Actor.h"
#include "Physics2D.h"
#include "ActorGroup.h"
#include "Camera2D.h"
#include "ChunkLoader.h"

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
	Vector2 playerPos = GetOwner()->GetTransform()->GetWorldPosition();
	Physics2D::Raycast(
		playerPos,
		MOUSE_WORLDPOS,
		500.f,
		ACTOR_GROUP::ENEMY
	);
	SDL_Log("Gun fired!");
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

void GunFiringComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::AddFloat(ofs, mDelay);
	FileIOHelper::AddInt(ofs, (int)mTriggerKey);
}

void GunFiringComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	mDelay = FileIOHelper::LoadFloat(ifs);
	mTriggerKey = (MOUSE)FileIOHelper::LoadInt(ifs);
}
