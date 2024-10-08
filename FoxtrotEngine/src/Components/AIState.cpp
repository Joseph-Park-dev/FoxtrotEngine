#include "Components/AIState.h"

#include "Components/AIComponent.h"
#include "Actors/Actor.h"
#include "Managers/SceneManager.h"
#include "Scenes/Scene.h"
#include "Components/MoveComponent.h"
#include "Actors/Transform.h"
#include "Core/EventFunctions.h"

AIPursue::AIPursue(AIComponent* owner)
	: AIState(owner)
	, mTargetActor(nullptr)
{
}

void AIIdle::Update(float deltaTime)
{
	//	Actor* player =
	// SceneManager::GetInstance()->GetCurrentScene()
	//	->GetActorGroup(ActorGroup::PLAYER)[0];
	// FTVector2 playerPos = player->GetTransform()->GetWorldPosition();

	// Asteroid* monster = (Asteroid*)GetOwner()->GetOwner();
	// FTVector2 monsterPos = monster->GetTransform()->GetWorldPosition();
	//
	// FTVector2 diff = playerPos - monsterPos;
	// float len = diff.Length();
	//
	// if (len < monster->GetMonsterInfo().RecogRange)
	//{
	//	ChangeAIState(GetOwner(), AISTATE_TYPE::PURSUE);
	// }
}

void AIIdle::OnEnter()
{
}

void AIIdle::OnExit()
{
}

void AIPatrol::Update(float deltaTime)
{
}

void AIPatrol::OnEnter()
{
}

void AIPatrol::OnExit()
{
}

void AIPursue::Update(float deltaTime)
{
	// if (mTargetActor != nullptr)
	//{
	//	FTVector2 playerPos = mTargetActor->GetTransform()->GetWorldPosition();

	//	Asteroid* monster = (Asteroid*)GetOwner()->GetOwner();
	//	FTVector2 monsterPos = monster->GetTransform()->GetWorldPosition();

	//	FTVector2 targetDir = FTVector2::Normalize(playerPos - monsterPos);
	//	monster->GetTransform()->Translate(targetDir * monster->GetMonsterInfo().Speed * deltaTime);
	//}
}

void AIPursue::OnEnter()
{
}

void AIPursue::OnExit()
{
}