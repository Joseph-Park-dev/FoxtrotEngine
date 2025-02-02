// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/BoxCollider2D.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Rigidbody2D.h"
#include "Renderer/Camera.h"
#include "Managers/CollisionManager.h"
#include "Actors/Transform.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
// Used to access Debug geometries
#include "ResourceSystem/GeometryGenerator.h"
#include "DebugShapes.h"
#endif

const FTVector2 BoxCollider2D::GetScale() const
{
    return mScale;
}

void BoxCollider2D::SetScale(FTVector2 scale)
{
	mScale = scale;
}

void BoxCollider2D::Initialize(FTCore* coreInstance)
{
#ifdef FOXTROT_EDITOR
	mDebugRect = DBG_NEW FTRectangle;
	mDebugRect->Initialize(coreInstance->GetGameRenderer());
	if (!IsShowingDebugShape())
		mDebugRect->SetIsActive(false);
#endif // FOXTROT_EDITOR

	Component::Initialize(coreInstance);
}

void BoxCollider2D::Setup()
{
	Rigidbody2D* rb = GetOwner()->GetComponent<Rigidbody2D>();
	if (rb)
	{
		if (b2Body_IsValid(rb->GetBodyID()))
		{
			b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
			// Setting the category of this collider as ActorGroup
			polygonShapeDef.filter.categoryBits = uint64_t(GetOwner()->GetActorGroup());
			CollisionManager::GetInstance()->MarkGroup(polygonShapeDef, GetOwner()->GetActorGroup());

			FTVector2 polygonScale = mScale * FTVector2(GetOwner()->GetTransform()->GetScale());
			b2Polygon polygon = b2MakeBox(polygonScale.x / 2, polygonScale.y / 2);

			GetShapeID() = b2CreatePolygonShape(rb->GetBodyID(), &polygonShapeDef, &polygon);
			CollisionManager::GetInstance()->RegisterCollider(GetShapeID().index1, this);
		}
		else
			LogString("ERROR : BoxCollider2D::Initialize() -> BodyId not valid");
	}
	Component::Setup();
}

void BoxCollider2D::CloneTo(Actor* actor)
{
	BoxCollider2D* newComp = DBG_NEW BoxCollider2D(actor, GetUpdateOrder());
	newComp->SetOffsetPos(this->GetOffsetPos());
	newComp->mScale = this->mScale;
}

BoxCollider2D::BoxCollider2D(Actor* owner, int updateOrder)
	: Collider2D(owner, updateOrder)
	, mScale(FTVector2(1.f, 1.f))
#ifdef FOXTROT_EDITOR
	, mDebugRect(nullptr)
#endif
{}

BoxCollider2D::~BoxCollider2D()
{
	if (b2Shape_IsValid(GetShapeID()))
		b2DestroyShape(GetShapeID(), true);
#ifdef FOXTROT_EDITOR
	DebugShapes::GetInstance()->RemoveShape(mDebugRect);
	mDebugRect = nullptr;
#endif
}

void BoxCollider2D::SaveProperties(std::ofstream& ofs)
{
	Collider2D::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::COLLIDER_SCALE, mScale);
}

void BoxCollider2D::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mScale);
	Collider2D::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void BoxCollider2D::EditorUpdate(float deltaTime)
{
	if (IsShowingDebugShape())
	{
		Transform* transform = GetOwner()->GetTransform();
		FTVector3 pos = transform->GetWorldPosition();
		pos = pos * FTVector3(1.f, -1.f, 1.f);
		FTVector3 rot = transform->GetRotation();
		FTVector3 scale = FTVector3(
			transform->GetScale().x * mScale.x,
			transform->GetScale().y * mScale.y,
			1.f
		);
		UpdateDebugShapes(pos, rot, scale, Camera::GetInstance());
	}
}

void BoxCollider2D::EditorRender(FoxtrotRenderer* renderer)
{
}

void BoxCollider2D::EditorUIUpdate()
{
	Collider2D::EditorUIUpdate();
	UpdateScale();
}

void BoxCollider2D::ToggleDebugShape()
{
	Collider2D::ToggleDebugShape();
	if(mDebugRect)
		mDebugRect->SetIsActive(IsShowingDebugShape());
}

void BoxCollider2D::UpdateDebugShapes(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* cameraInst)
{
	/*float NDC = Camera::GetInstance()->GetNDCRatio();
	pos *= NDC;
	scale *= NDC;*/

	if (mDebugRect->GetIsActive())
	{
		mDebugRect->Update(
			pos,
			rot,
			scale,
			Camera::GetInstance()
		);
	}
}

void BoxCollider2D::UpdateScale()
{
	FTVector2 updatedVal = GetScale();
	CommandHistory::GetInstance()->UpdateVector2Value("Scale", updatedVal, FLOATMOD_SPEED);
	if (mScale != updatedVal)
	{
		mScale = updatedVal;
		if (mScale.x <= 0 || mScale.y <= 0)
			return;

		FTVector2 resultantScale = mScale * FTVector2(GetOwner()->GetTransform()->GetScale());
		b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
		b2Polygon polygon = b2MakeBox(resultantScale.x / 2, resultantScale.y / 2);
		if (b2Shape_IsValid(GetShapeID()))
		{
			//b2DestroyShape(GetShapeID(), true);
			b2Shape_SetPolygon(GetShapeID(), &polygon);
		}
	}
}
#endif // FOXTROT_EDITOR