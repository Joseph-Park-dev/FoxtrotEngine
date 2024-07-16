#include "Scripts/ActorList/Asteroid.h"

#include "FoxtrotEngine/Components/SpriteRendererComponent.h"
#include "FoxtrotEngine/Components/MoveComponent.h"
#include "FoxtrotEngine/Math/Random.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Components/ColliderComponent.h"
#include "FoxtrotEngine/Components/AIComponent.h"
#include "FoxtrotEngine/Components/AIState.h"
#include "FoxtrotEngine/Actors/ActorTag.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/Camera2D.h"
#include "FoxtrotEngine/Physics/ParticleSystem.h"

//Asteroid::Asteroid(Scene* scene)
//	: Actor(scene)
//{
//}
//
//Asteroid::Asteroid(Scene* scene, tMonsterInfo info)
//	: Actor(scene)
//	, monsterInfo{}
//{
//	//GetTransform()->SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));
//
//	SpriteRendererComponent* sc = new SpriteRendererComponent(this);
//	/*sc->SetTexture(
//		ResourceManager::GetInstance()->GetLoadedTexture(L"Enemy01")
//	);*/
//	MoveComponent* mc = new MoveComponent(this, 100, 53);
//	//mc->SetForwardSpeed(info.Speed);
//
//	ColliderComponent* collider = new ColliderComponent(this, 100, 54);
//	collider->SetScale(FTVector2(50.f, 50.f));
//}
//
//Asteroid::~Asteroid()
//{
//}
//
//void Asteroid::OnRayEnter()
//{
//	if (monsterInfo.HP <= 0)
//	{
//		Camera2D::GetInstance()->Shake(0.05f, 10.f);
//		ParticleSystem::GetInstance()->CreateExplosion(
//			GetTransform()->GetWorldPosition(),
//			100.f, 3.f, 9.81f, 999.f);
//		this->SetState(Actor::State::EDead);
//		return;
//	}
//	--monsterInfo.HP;
//}

Asteroid::Asteroid(Scene* scene)
	: Actor(scene)
{
}

Asteroid::Asteroid(Scene* scene, tMonsterInfo info)
	: Actor(scene)
{
}

Asteroid::~Asteroid()
{
}

void Asteroid::OnRayEnter()
{
}
