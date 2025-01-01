// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/CircleCollider2DComponent.h"

#include "Components/Rigidbody2DComponent.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"

const float CircleCollider2DComponent::GetRadius() const
{
    return mRadius;
}

void CircleCollider2DComponent::SetRadius(float radius)
{
    mRadius = radius;
}

void CircleCollider2DComponent::Initialize(FTCore* coreInstance)
{
	Rigidbody2DComponent* rb = GetOwner()->GetComponent<Rigidbody2DComponent>();
	if (rb)
	{
		if (b2Body_IsValid(rb->GetBodyID()))
		{
			FTVector2 center = GetFinalPosition();
			b2Circle circle = b2Circle{ { center.x, center.y }, mRadius };
			b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
			b2CreateCircleShape(rb->GetBodyID(), &polygonShapeDef, &circle);
		}
		else
			LogString("ERROR : CircleCollider2DComponent::Initialize() -> BodyId not valid");
	}
    Component::Initialize(coreInstance);
}

CircleCollider2DComponent::CircleCollider2DComponent(Actor* owner, int drawOrder, int updateOrder)
	: Collider2DComponent(owner, drawOrder, updateOrder)
	, mRadius(50.f)
{
}

void CircleCollider2DComponent::SaveProperties(std::ofstream& ofs)
{
}

void CircleCollider2DComponent::LoadProperties(std::ifstream& ifs)
{
}

#ifdef FOXTROT_EDITOR
void CircleCollider2DComponent::EditorUpdate(float deltaTime)
{
}

void CircleCollider2DComponent::EditorRender(FoxtrotRenderer* renderer)
{

}

void CircleCollider2DComponent::EditorUIUpdate()
{
}

void CircleCollider2DComponent::UpdateScale()
{
}
#endif // FOXTROT_EDITOR