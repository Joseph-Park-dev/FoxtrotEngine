#include "Scripts/ActorList/GroundObject.h"

#include "FoxtrotEngine/Components/ColliderComponent.h"
#include "FoxtrotEngine/Components/Rigidbody2DComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/ActorGroup.h"
#include "FoxtrotEngine/Physics/Physics2D.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"

void GroundObject::OnCollisionEnter(ColliderComponent* other)
{
	if (other->GetOwner()->GetActorGroup() == ACTOR_GROUP::PLAYER)
	{
		if (other->GetCollidedSide() == CollidedSide::DOWN)
		{
			Actor* otherActor = other->GetOwner();
			otherActor->GetComponent<Rigidbody2DComponent>()->SetIsGrounded(true);
		}
		if (other->GetCollidedSide() == CollidedSide::UP)
		{
			Actor* otherActor = other->GetOwner();
			otherActor->GetComponent<Rigidbody2DComponent>()->SetIsBlockedUp(true);
		}
	}
}

void GroundObject::OnCollisionStay(ColliderComponent* other)
{
}

void GroundObject::DirectionalBlocking(ColliderComponent* other)
{
}

void GroundObject::OnCollisionExit(ColliderComponent* other)
{
	if (other->GetOwner()->GetActorGroup() == ACTOR_GROUP::PLAYER)
	{
		Actor* otherActor = other->GetOwner();
		otherActor->GetComponent<Rigidbody2DComponent>()->SetIsGrounded(false);
		otherActor->GetComponent<Rigidbody2DComponent>()->SetIsBlockedUp(false);
	}
}

void GroundObject::OnRayEnter()
{
	
}

void GroundObject::UpdateActor(float deltaTime)
{
	//Vector2 screenPos = GetTransform()->GetScreenPosition();
	//rect.x = screenPos.x;
	//rect.y = screenPos.y;
	//rect.w = GetTransform()->GetScale().x;
	//rect.h = GetTransform()->GetScale().y;
	//collider->SetScale(Vector2((int)GetTransform()->GetScale().x, (int)GetTransform()->GetScale().y));
}

GroundObject::GroundObject(class Scene* scene)
	: Actor(scene)
{
}

GroundObject::~GroundObject()
{
}

void GroundObject::Initialize()
{
}