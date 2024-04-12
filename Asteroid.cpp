#include "Asteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Random.h"
#include "ResourceManager.h"
#include "ColliderComponent.h"
#include "AIComponent.h"
#include "AIState.h"
#include "ActorTag.h"
#include "Transform.h"
#include "Camera2D.h"
#include "ParticleSystem.h"

Asteroid::Asteroid(Scene* scene)
	: Actor(scene)
{
}

Asteroid::Asteroid(Scene* scene, tMonsterInfo info)
	: Actor(scene)
	, monsterInfo{}
{
	GetTransform()->SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(
		ResourceManager::GetInstance()->GetLoadedTexture(L"Enemy01")
	);
	MoveComponent* mc = new MoveComponent(this, 100, 53);
	//mc->SetForwardSpeed(info.Speed);

	ColliderComponent* collider = new ColliderComponent(this, 100, 54);
	collider->SetScale(Vector2(50.f, 50.f));
}

Asteroid::~Asteroid()
{
}

void Asteroid::OnRayEnter()
{
	if (monsterInfo.HP <= 0)
	{
		Camera2D::GetInstance()->Shake(0.05f, 10.f);
		ParticleSystem::GetInstance()->CreateExplosion(
			GetTransform()->GetWorldPosition(),
			100.f, 3.f, 9.81f, 999.f);
		this->SetState(Actor::State::EDead);
		return;
	}
	--monsterInfo.HP;
}