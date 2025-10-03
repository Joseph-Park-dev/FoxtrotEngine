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
	DebugShapes::GetInstance()->AddShape(mDebugRect);
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

			FTVector3 pos	  = GetOffsetPos();
			FTVector3 rot	  = GetOwner()->GetTransform()->GetWorldRotation();
			float	  rotZ	  = Transform::ConvertDegreeToRad(rot).z;
			b2Polygon polygon = b2MakeOffsetBox(
				mSize.x / 2,
				mSize.y / 2,
				pos.GetB2Vec2(),
				b2Rot_identity);

			GetShapeID() = b2CreatePolygonShape(rb->GetBodyID(), &polygonShapeDef, &polygon);
			CollisionManager::GetInstance()->RegisterCollider(GetShapeID().index1, this);
		}
		else
			LogString("ERROR : BoxCollider2D::Initialize() -> BodyId not valid");
	}
	Component::Setup();
}

void BoxCollider2D::Render(FoxtrotRenderer* renderer)
{
	UpdateDebugShape(Camera::GetInstance());
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
	, mDebugRect(nullptr)
{
}

BoxCollider2D::~BoxCollider2D()
{
	CollisionManager::GetInstance()->RemoveCollider(GetShapeID().index1);
	if (b2Shape_IsValid(GetShapeID()))
		b2DestroyShape(GetShapeID(), true);

	DebugShapes::GetInstance()->RemoveShape(mDebugRect);
	mDebugRect = nullptr;
}

void BoxCollider2D::UpdateDebugShape(Camera* camInst)
{
	if (!mDebugRect)
		return;

	if (mDebugRect)
		mDebugRect->SetIsActive(IsShowingDebugShape());

	if (IsShowingDebugShape())
	{
		Transform* transform	  = GetOwner()->GetTransform();
		FTVector2  offset		  = GetOffsetPos();
		Matrix	   translationMat = Matrix::CreateTranslation(offset.x, offset.y, 0.0f);
		Matrix	   modelMat		  = transform->GetMatrixWorld() * translationMat;

		mDebugRect->UpdateVC(modelMat, camInst);
		mDebugRect->UpdateGC(camInst);
		mDebugRect->GetGSCData().size.x = mSize.x;
		mDebugRect->GetGSCData().size.y = mSize.y;
	}
	mDebugRect->UpdatePC();
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
}

void BoxCollider2D::EditorRender(FoxtrotRenderer* renderer)
{
	UpdateDebugShape(EditorCamera::GetInstance());
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
	}
}
#endif // FOXTROT_EDITOR