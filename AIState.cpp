#include "AIComponent.h"
#include "AIState.h"
#include "SDL2/SDL.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Asteroid.h"
#include "MoveComponent.h"
#include "Transform.h"

AIPursue::AIPursue(AIComponent* owner)
	: AIState(owner)
	, mTargetActor(nullptr)
{}

void AIIdle::Update(float deltaTime)
{
 	Actor* player =
	SceneManager::GetInstance()->GetCurrScene()
		->GetActorGroup(ACTOR_GROUP::PLAYER)[0];
	Vector2 playerPos = player->GetTransform()->GetWorldPosition();

	Asteroid* monster = (Asteroid*)GetOwner()->GetOwner();
	Vector2 monsterPos = monster->GetTransform()->GetWorldPosition();
	
	Vector2 diff = playerPos - monsterPos;
	float len = diff.Length();
	
	if (len < monster->GetMonsterInfo().RecogRange)
	{
		ChangeAIState(GetOwner(), AISTATE_TYPE::PURSUE);
	}
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
	if (mTargetActor != nullptr)
	{
		Vector2 playerPos = mTargetActor->GetTransform()->GetWorldPosition();

		Asteroid* monster = (Asteroid*)GetOwner()->GetOwner();
		Vector2 monsterPos = monster->GetTransform()->GetWorldPosition();

		Vector2 targetDir = Vector2::Normalize(playerPos - monsterPos);
		monster->GetTransform()->Translate(targetDir * monster->GetMonsterInfo().Speed * deltaTime);
	}
}

void AIPursue::OnEnter()
{
	mTargetActor =
		SceneManager::GetInstance()->GetCurrScene()
		->GetActorGroup(ACTOR_GROUP::PLAYER)[0];
}

void AIPursue::OnExit()
{
}