// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/Collider2D.h"

#include <assert.h>
#include <DirectXColors.h>

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"

#include "Physics/Physics2D.h"
#include "Managers/KeyInputManager.h"
#include "Managers/CollisionManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Rigidbody2D.h"
#include "Behaviors/FTBehavior.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "CommandHistory.h"
#endif // FOXTROT_EDITOR


b2ShapeId& Collider2D::GetShapeID()
{
	return mShapeID;
}

FTVector2 Collider2D::GetOffsetPos() const
{
	return mOffset;
}

FTVector2 Collider2D::GetFinalPosition() const
{
	return mFinalPosition;
}

CollisionStatesMap& Collider2D::GetCollisionStates()
{
	return mCollisionStates;
}

FTVector2& Collider2D::GetOffsetPosRef()
{
	return mOffset;
}

void Collider2D::SetOffsetPos(FTVector2 offsetPos)
{
	mOffset = offsetPos;
}

void Collider2D::Initialize(FTCore* coreInstance)
{
}

void Collider2D::LateUpdate(float deltaTime)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffset;
}

void Collider2D::CloneTo(Actor* actor)
{}

Collider2D::Collider2D(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mOffset(FTVector2::Zero)
	, mFinalPosition(FTVector2::Zero)
	, mShapeID(b2_nullShapeId)
#ifdef FOXTROT_EDITOR
	, mShowDebugShape(false)
#endif // FOXTROT_EDITOR

{
}

Collider2D::Collider2D(const Collider2D& origin)
	: Component(origin.GetOwner(), origin.GetUpdateOrder())
	, mShapeID(b2_nullShapeId)
	, mOffset(origin.mOffset)
	, mShowDebugShape(false)
{
}

Collider2D::~Collider2D()
{
	if (b2Shape_IsValid(mShapeID))
		b2DestroyShape(mShapeID, true);
}

void Collider2D::OnCollisionEnter(Collider2D* other)
{
	std::vector<Component*>::iterator iter = GetOwner()->GetComponents().begin();
	for (; iter != GetOwner()->GetComponents().end(); ++iter)
	{
		FTBehavior* behav = dynamic_cast<FTBehavior*>(*iter);
		if (behav)
			behav->OnCollisionEnter(other);
	}
}

void Collider2D::OnCollisionStay(Collider2D* other)
{
	std::vector<Component*>::iterator iter = GetOwner()->GetComponents().begin();
	for (; iter != GetOwner()->GetComponents().end(); ++iter)
	{
		FTBehavior* behav = dynamic_cast<FTBehavior*>(*iter);
		if (behav)
			behav->OnCollisionStay(other);
	}
}

void Collider2D::OnCollisionExit(Collider2D* other)
{
	std::vector<Component*>::iterator iter = GetOwner()->GetComponents().begin();
	for (; iter != GetOwner()->GetComponents().end(); ++iter)
	{
		FTBehavior* behav = dynamic_cast<FTBehavior*>(*iter);
		if (behav)
			behav->OnCollisionExit(other);
	}
}

void Collider2D::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::OFFSET, mOffset);
}

void Collider2D::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mOffset);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void Collider2D::EditorUpdate(float deltaTime)
{
}

void Collider2D::EditorRender(FoxtrotRenderer* renderer)
{

}

void Collider2D::EditorUIUpdate()
{
	ToggleDebugShape();
	UpdateOffsetPos();
}

void Collider2D::UpdateOffsetPos()
{
	FTVector2 updatedVal = GetOffsetPos();
	CommandHistory::GetInstance()->UpdateVector2Value("Offset Position", updatedVal, FLOATMOD_SPEED);
	mOffset = updatedVal;
}

void Collider2D::ToggleDebugShape()
{
	static bool showDebugShape;
	ImGui::Checkbox("Show Debug Shape", &showDebugShape);
	mShowDebugShape = showDebugShape;
}

bool Collider2D::IsShowingDebugShape()
{
	return mShowDebugShape;
}
#endif