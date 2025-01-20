// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/CircleCollider2D.h"

#include "Components/Rigidbody2D.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"

const float CircleCollider2D::GetRadius() const
{
    return mRadius;
}

void CircleCollider2D::SetRadius(float radius)
{
    mRadius = radius;
}

void CircleCollider2D::Initialize(FTCore* coreInstance)
{
	Rigidbody2D* rb = GetOwner()->GetComponent<Rigidbody2D>();
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
			LogString("ERROR : CircleCollider2D::Initialize() -> BodyId not valid");
	}
	Component::Initialize(coreInstance);
}

void CircleCollider2D::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}

CircleCollider2D::CircleCollider2D(Actor* owner, int updateOrder)
	: Collider2D(owner, updateOrder)
	, mRadius(50.f)
{
}

void CircleCollider2D::SaveProperties(std::ofstream& ofs)
{
}

void CircleCollider2D::LoadProperties(std::ifstream& ifs)
{
}

#ifdef FOXTROT_EDITOR
void CircleCollider2D::EditorUpdate(float deltaTime)
{
}

void CircleCollider2D::EditorRender(FoxtrotRenderer* renderer)
{

}

void CircleCollider2D::EditorUIUpdate()
{
}

void CircleCollider2D::UpdateScale()
{
}
#endif // FOXTROT_EDITOR