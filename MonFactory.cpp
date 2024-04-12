#include "MonFactory.h"
#include "Asteroid.h"
#include "Scene.h"
#include "AIComponent.h"
#include "AIState.h"
#include "Transform.h"

Asteroid* MonFactory::CreateMonster(Scene* scene, Vector2 pos, MON_TYPE type)
{
	Asteroid* asteroid = nullptr;
	switch (type)
	{
	case MON_TYPE::NORMAL:
	{
		tMonsterInfo info = {};
		info.AttacDamage = 10.f;
		info.AttacRange = 10.f;
		info.HP = 10.f;
		info.RecogRange = 100.f;
		info.Speed = 150.f;
		asteroid = new Asteroid(scene, info);
		asteroid->GetTransform()->SetWorldPosition(pos);
		asteroid->SetMonsterInfo(info);

		AIComponent* ai = new AIComponent(asteroid);
		ai->RegisterState(new AIIdle(ai));
		ai->RegisterState(new AIPursue(ai));
		ai->ChangeState(AISTATE_TYPE::IDLE);
	}
	break;
	}
	return asteroid;
}
