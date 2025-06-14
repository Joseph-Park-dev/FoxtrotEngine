// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/BoxCollider2D.h"

#include "Components/Rigidbody2D.h"
#include "Renderer/Camera.h"
#include "Renderer/FTRectArea.h"
#include "Managers/CollisionManager.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/FTRectangle.h"
#include "Managers/DebugShapes.h"
#include "ResourceSystem/GeometryGenerator.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorCamera.h"
#endif

const FTVector2 BoxCollider2D::GetSize() const
{
	return mSize;
}

void BoxCollider2D::SetSize(FTVector2 size)
{
	mSize = size;
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

			FTVector2 polygonScale = mSize * FTVector2(GetOwner()->GetTransform()->GetWorldScale());
			b2Polygon polygon	   = b2MakeBox(polygonScale.x / 2, polygonScale.y / 2);
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
	newComp->mSize				   = this->mSize;
	newComp->IsShowingDebugShape() = this->IsShowingDebugShape();
}

BoxCollider2D::BoxCollider2D(Actor* owner, int updateOrder)
	: Collider2D(owner, updateOrder)
	, mSize(FTVector2(0.f, 0.f))
#ifdef FOXTROT_EDITOR
	, mDebugRect(nullptr)
#endif
{
}

BoxCollider2D::~BoxCollider2D()
{
	CollisionManager::GetInstance()->RemoveCollider(GetShapeID().index1);
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
	FileIOHelper::SaveVector2(ofs, ChunkKey::COLLIDER_SCALE, mSize);
}

void BoxCollider2D::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mSize);
	Collider2D::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void BoxCollider2D::EditorUpdate(float deltaTime)
{
	if (mDebugRect)
		mDebugRect->SetIsActive(IsShowingDebugShape());

	if (IsShowingDebugShape())
	{
		Transform* transform = GetOwner()->GetTransform();
		FTVector3  pos		 = transform->GetWorldPosition();
		// pos					 = pos * FTVector3(1.f, -1.f, 1.f);
		FTVector3 rot  = transform->GetWorldRotation();
		FTVector3 size = FTVector3(
			transform->GetWorldScale().x * mSize.x,
			transform->GetWorldScale().y * mSize.y,
			1.f);
		mDebugRect->UpdateVC(pos, rot, size, EditorCamera::GetInstance());
		mDebugRect->GetGSCData().size.x = size.x;
		mDebugRect->GetGSCData().size.y = size.y;
	}
	mDebugRect->UpdatePC();
}

void BoxCollider2D::EditorRender(FoxtrotRenderer* renderer)
{
}

void BoxCollider2D::EditorUIUpdate()
{
	Collider2D::EditorUIUpdate();
	UpdateScale();
}

void BoxCollider2D::UpdateScale()
{
	FTVector2 updatedVal = mSize;
	CommandHistory::GetInstance()->UpdateVector2Value("Scale", updatedVal, FLOATMOD_SPEED);
	if (mSize != updatedVal)
	{
		mSize = updatedVal;
		if (mSize.x <= 0 || mSize.y <= 0)
			return;

		FTVector2  resultantScale  = mSize * FTVector2(GetOwner()->GetTransform()->GetWorldScale());
		b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
		b2Polygon  polygon		   = b2MakeBox(resultantScale.x / 2, resultantScale.y / 2);
		if (b2Shape_IsValid(GetShapeID()))
		{
			// b2DestroyShape(GetShapeID(), true);
			b2Shape_SetPolygon(GetShapeID(), &polygon);
		}
	}
}
#endif // FOXTROT_EDITOR