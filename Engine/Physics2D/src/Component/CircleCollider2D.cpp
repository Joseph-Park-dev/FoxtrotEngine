// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/CircleCollider2D.h"
#include "Plugin/Physics2D.h"

#include "Actor/ActorData.h"
#include "Actor/IActor.h"
#include "Component/Rigidbody2D.h"
#include "Debugging/DebugMemAlloc.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "TemplateFunctions.h"

namespace Physics2D
{
	const float CircleCollider2D::GetRadius() const
	{
		return mRadius;
	}

	void CircleCollider2D::SetRadius(float radius)
	{
		mRadius = radius;
	}

	void CircleCollider2D::Initialize()
	{
		if (b2Shape_IsValid(GetShapeID()))
			return;

		Rigidbody2D* rb = nullptr;
		if (GetOwner() && GetOwner()->GetData() && GetOwner()->GetData()->Components)
			rb = GetOwner()->GetData()->GetComponent<Rigidbody2D>();

		if (rb && b2Body_IsValid(rb->GetBodyID()))
		{
			const Math::FTVector2 center = GetOffsetPos();
			b2Circle				circle = b2Circle{ { center.x, center.y }, mRadius };
			b2ShapeDef				shapeDef = b2DefaultShapeDef();
			GetShapeID()					 = b2CreateCircleShape(rb->GetBodyID(), &shapeDef, &circle);
		}
		else if (rb)
		{
			Common::LogString("ERROR : CircleCollider2D::Initialize() -> BodyId not valid");
		}

		Collider2D::Initialize();
	}

	CircleCollider2D::CircleCollider2D(Common::IActor* owner)
		: Collider2D(owner)
		, mCenter(0.f, 0.f)
		, mRadius(50.f)
	{
	}

	CircleCollider2D::~CircleCollider2D()
	{
	}

	void CircleCollider2D::CloneTo(Common::IActor* actor)
	{
		CircleCollider2D* newComp = DBG_NEW CircleCollider2D(actor);
		newComp->SetOffsetPos(GetOffsetPos());
		newComp->mRadius = mRadius;
		newComp->mCenter = mCenter;
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(newComp);
		RegisterOwnedComponent(newComp);
	}

	void CircleCollider2D::SaveProperties(std::ofstream& ofs)
	{
		Collider2D::SaveProperties(ofs);
		(void)ofs;
	}

	void CircleCollider2D::LoadProperties(std::ifstream& ifs)
	{
		Collider2D::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void CircleCollider2D::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void CircleCollider2D::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	void CircleCollider2D::EditorUIUpdate()
	{
	}
#endif
} // namespace Physics2D
