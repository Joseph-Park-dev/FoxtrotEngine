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
#include "ResourceSystem/FTRectangle.h"
#include "Managers/DebugShapes.h"
#include "Managers/CollisionManager.h"
#include "Renderer/Camera.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorCamera.h"
#endif

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
#ifdef FOXTROT_EDITOR
	mDebugRect = DBG_NEW FTRectangle;
	mDebugRect->Initialize(coreInstance->GetGameRenderer());
	DebugShapes::GetInstance()->AddShape(mDebugRect);
	if (!IsShowingDebugShape())
		mDebugRect->SetIsActive(false);
#endif // FOXTROT_EDITOR

	Component::Initialize(coreInstance);
}

void CircleCollider2D::Setup()
{
	Rigidbody2D* rb = GetOwner()->GetComponent<Rigidbody2D>();
	if (rb)
	{
		if (b2Body_IsValid(rb->GetBodyID()))
		{
			FTVector2  center					= GetFinalPosition();
			b2ShapeDef polygonShapeDef			= b2DefaultShapeDef();
			polygonShapeDef.filter.categoryBits = uint64_t(GetOwner()->GetActorGroup());
			CollisionManager::GetInstance()->MarkGroup(polygonShapeDef, GetOwner()->GetActorGroup());

			b2Circle circle = b2Circle{ { center.x, center.y }, mRadius };
			GetShapeID()	= b2CreateCircleShape(rb->GetBodyID(), &polygonShapeDef, &circle);
			CollisionManager::GetInstance()->RegisterCollider(GetShapeID().index1, this);
		}
		else
			LogString("ERROR : CircleCollider2D::Initialize() -> BodyId not valid");
	}
	Component::Setup();
}

void CircleCollider2D::Render(FoxtrotRenderer* renderer)
{
	UpdateDebugShape(Camera::GetInstance());
}

void CircleCollider2D::CloneTo(Actor* actor)
{
	CircleCollider2D* newComp = DBG_NEW CircleCollider2D(actor, GetUpdateOrder());
	newComp->SetOffsetPos(this->GetOffsetPos());
	newComp->mRadius			   = this->mRadius;
	newComp->IsShowingDebugShape() = this->IsShowingDebugShape();
}

CircleCollider2D::CircleCollider2D(Actor* owner, int updateOrder)
	: Collider2D(owner, updateOrder)
	, mRadius(50.f)
	, mDebugRect(nullptr)
{
}

void CircleCollider2D::UpdateDebugShape(Camera* camInst)
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
		mDebugRect->GetGSCData().size.x = mRadius;
		mDebugRect->GetGSCData().size.y = mRadius;
	}
	mDebugRect->UpdatePC();
}

void CircleCollider2D::SaveProperties(std::ofstream& ofs)
{
	Collider2D::SaveProperties(ofs);
	FileIOHelper::SaveFloat(ofs, ChunkKey::COLLIDER_RADIUS, mRadius);
}

void CircleCollider2D::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadFloat(ifs, mRadius);
	Collider2D::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void CircleCollider2D::EditorUpdate(float deltaTime)
{
}

void CircleCollider2D::EditorRender(FoxtrotRenderer* renderer)
{
	UpdateDebugShape(EditorCamera::GetInstance());
}

void CircleCollider2D::EditorUIUpdate()
{
	Collider2D::EditorUIUpdate();
	UpdateScale();
}

void CircleCollider2D::UpdateScale()
{
	float updatedVal = mRadius;
	CommandHistory::GetInstance()->UpdateFloatValue("Radius", updatedVal, FLOATMOD_SPEED);
	if (mRadius != updatedVal)
	{
		mRadius = updatedVal;
		if (mRadius <= 0)
			return;
	}
}
#endif // FOXTROT_EDITOR