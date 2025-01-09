// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/GunFiringBehavior.h"

#include "Managers/KeyInputManager.h"
#include "Managers/ResourceManager.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Actors/Actor.h"
#include "Physics/Physics2D.h"
#include "ActorBehaviors/Bullet/BulletBehavior.h"

#include "FileSystem/ChunkLoader.h"
#include "Core/EventFunctions.h"

#ifdef FOXTROT_EDITOR
#include "EditorSceneManager.h"
#include "EditorLayer.h"
#endif // FOXTROT_EDITOR


void GunFiringBehavior::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void GunFiringBehavior::ProcessInput(KeyInputManager* keyInputManager)
{
	mDirection = CalcDirection(GetOwner()->GetTransform()->GetWorldPosition(), MOUSE_WORLDPOS);
}

void GunFiringBehavior::Update(float deltaTime)
{
	if (MOUSE_HOLD(MOUSE::MOUSE_LEFT))
	{
		if (mCurrentTick == 0)
		{
			ShootGun();
			mCurrentTick = mDelay;
		}
	}

	mCurrentTick -= deltaTime;

	if (mCurrentTick < 0)
		mCurrentTick = 0;
}

void GunFiringBehavior::CloneTo(Actor* actor)
{
	GunFiringBehavior* newComp = DBG_NEW GunFiringBehavior(actor, GetDrawOrder(), GetUpdateOrder());
}

void GunFiringBehavior::ShootGun()
{
#ifdef FOXTROT_EDITOR
	if (!EditorLayer::GetInstance()->CursorOnViewport())
		return;
#endif // FOXTROT_EDITOR
	Actor* bulletCopy = Instantiate("Bullet");
	bulletCopy->GetComponent<BulletBehavior>()->SetDirection(mDirection);
	bulletCopy->GetComponent<BulletBehavior>()->SetStartPosition(
		GetOwner()->GetTransform()->GetWorldPosition()
	);
	LogVector3("vec : ", GetOwner()->GetTransform()->GetWorldPosition());
}

GunFiringBehavior::GunFiringBehavior(Actor* owner, int drawOrder, int updateorder)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(MOUSE::MOUSE_LEFT)
	, mDelay(0.2f)
	, mCurrentTick(0.f)
	, mDirection(FTVector2(1.0f,0.f))
{}

GunFiringBehavior::GunFiringBehavior(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(triggerKey)
	, mDelay(delay)
	, mCurrentTick(0.f)
{}

GunFiringBehavior::~GunFiringBehavior()
{
}

FTVector2&& GunFiringBehavior::CalcDirection(FTVector2&& origin, FTVector2&& point)
{
	return FTVector2(point.x - origin.x, point.y + origin.y);
}

void GunFiringBehavior::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}