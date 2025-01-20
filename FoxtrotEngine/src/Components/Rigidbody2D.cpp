// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/Rigidbody2D.h"

#include "box2d/box2d.h"

#include "Physics/Physics2D.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Math/FTMath.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Components/BoxCollider2D.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR


Rigidbody2D::Rigidbody2D(class Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mBodyID{}
#ifdef FOXTROT_EDITOR
	, mBodyDefCache(b2DefaultBodyDef())
#endif // FOXTROT_EDITOR
{}

Rigidbody2D::~Rigidbody2D()
{
	if(b2Body_IsValid(mBodyID))
		b2DestroyBody(mBodyID);
}

b2BodyId& Rigidbody2D::GetBodyID()
{
	return mBodyID;
}

void Rigidbody2D::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void Rigidbody2D::LateUpdate(float deltaTime)
{
	b2Vec2 position = b2Body_GetPosition(mBodyID);
	b2Rot  rotation = b2Body_GetRotation(mBodyID);

	GetOwner()->GetTransform()->SetWorldPosition(FTVector2(position.x, position.y));
	float rotZ = b2Rot_GetAngle(rotation);
	FTVector3 prevRot = GetOwner()->GetTransform()->GetRotation();
	FTVector3 updatedRot = FTVector3(prevRot.x, prevRot.y, -rotZ);
	GetOwner()->GetTransform()->SetRotation(updatedRot);
}

void Rigidbody2D::CloneTo(Actor* actor)
{
	Rigidbody2D* newComp = DBG_NEW Rigidbody2D(actor, GetUpdateOrder());
	newComp->mBodyDefCache = this->mBodyDefCache;
	newComp->mBodyID = b2CreateBody(Physics2D::GetInstance()->GetCurrentWorldID(), &mBodyDefCache);
}

void Rigidbody2D::LoadProperties(std::ifstream& ifs)
{
	if (b2Body_IsValid(mBodyID))
		b2DestroyBody(mBodyID);

	b2BodyDef bodyDef = b2DefaultBodyDef();

	int bodyTypeInt = VALUE_NOT_ASSIGNED;
	FileIOHelper::LoadInt(ifs, bodyTypeInt);
	bodyDef.type = static_cast<b2BodyType>(bodyTypeInt);

	FileIOHelper::LoadFloat		(ifs, bodyDef.sleepThreshold);
	FileIOHelper::LoadVector2	(ifs, bodyDef.linearVelocity);
	FileIOHelper::LoadBool		(ifs, bodyDef.isEnabled);
	FileIOHelper::LoadBool		(ifs, bodyDef.isBullet);
	FileIOHelper::LoadBool		(ifs, bodyDef.isAwake);
	FileIOHelper::LoadFloat		(ifs, bodyDef.gravityScale);
	FileIOHelper::LoadBool		(ifs, bodyDef.fixedRotation);
	FileIOHelper::LoadBool		(ifs, bodyDef.enableSleep);
	FileIOHelper::LoadFloat		(ifs, bodyDef.angularVelocity);
	FileIOHelper::LoadFloat		(ifs, bodyDef.angularDamping);
	FileIOHelper::LoadBool		(ifs, bodyDef.allowFastRotation);
	Component::LoadProperties(ifs);
	
	// Initialize transform-related body definitions
	bodyDef.position = GetOwner()->GetTransform()->GetWorldPosition().GetB2Vec2();
	bodyDef.rotation = b2MakeRot(-GetOwner()->GetTransform()->GetRotation().z);

	mBodyID = b2CreateBody(Physics2D::GetInstance()->GetCurrentWorldID(), &bodyDef);

#ifdef FOXTROT_EDITOR
	mBodyDefCache = bodyDef;
#endif // FOXTROT_EDITOR
}
#ifdef FOXTROT_EDITOR
void Rigidbody2D::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties (ofs);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::ALLOW_FAST_ROTATION, mBodyDefCache.allowFastRotation);
	FileIOHelper::SaveFloat	  (ofs, ChunkKeys::ANGULAR_DAMPING, mBodyDefCache.angularDamping);
	FileIOHelper::SaveFloat	  (ofs, ChunkKeys::ANGULAR_VELOCITY, mBodyDefCache.angularVelocity);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::ENABLE_SLEEP, mBodyDefCache.enableSleep);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::FIXED_ROTATION, mBodyDefCache.fixedRotation);
	FileIOHelper::SaveFloat	  (ofs, ChunkKeys::GRAVITY_SCALE, mBodyDefCache.gravityScale);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::IS_AWAKE, mBodyDefCache.isAwake);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::IS_BULLET, mBodyDefCache.isBullet);
	FileIOHelper::SaveBool	  (ofs, ChunkKeys::IS_ENABLED, mBodyDefCache.isEnabled);
	FileIOHelper::SaveVector2 (ofs, ChunkKeys::INITIAL_VELOCITY, mBodyDefCache.linearVelocity);
	FileIOHelper::SaveFloat	  (ofs, ChunkKeys::SLEEP_THRESHOLD, mBodyDefCache.sleepThreshold);
	switch (mBodyDefCache.type)
	{
	case b2_staticBody:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_STATIC);
		break;
	case b2_kinematicBody:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_KINEMATIC);
		break;
	case b2_dynamicBody:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_DYNAMIC);
		break;
	default:
		break;
	}
}

void Rigidbody2D::EditorUpdate(float deltaTime)
{
}

void Rigidbody2D::EditorRender(FoxtrotRenderer* renderer)
{
}

void Rigidbody2D::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::ALLOW_FAST_ROTATION, mBodyDefCache.allowFastRotation);
	CommandHistory::GetInstance()->UpdateFloatValue	  (ChunkKeys::ANGULAR_DAMPING,	  &mBodyDefCache.angularDamping,  FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue	  (ChunkKeys::ANGULAR_VELOCITY,   &mBodyDefCache.angularVelocity, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::ENABLE_SLEEP,		   mBodyDefCache.enableSleep);
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::FIXED_ROTATION,	   mBodyDefCache.fixedRotation);
	CommandHistory::GetInstance()->UpdateFloatValue	  (ChunkKeys::GRAVITY_SCALE,	  &mBodyDefCache.gravityScale,	  FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::IS_AWAKE,			   mBodyDefCache.isAwake);
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::IS_BULLET,		   mBodyDefCache.isBullet);
	CommandHistory::GetInstance()->UpdateBoolValue	  (ChunkKeys::IS_ENABLED,		   mBodyDefCache.isEnabled);
	CommandHistory::GetInstance()->UpdateVector2Value (ChunkKeys::INITIAL_VELOCITY,    mBodyDefCache.linearVelocity,  FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue	  (ChunkKeys::SLEEP_THRESHOLD,    &mBodyDefCache.sleepThreshold,  FLOATMOD_SPEED);
	UpdateBodyType();

	ImGui::SeparatorText("Body Info");
	mBodyDefCache.position = GetOwner()->GetTransform()->GetWorldPosition().GetB2Vec2();
	std::string pos = {
		"x : " + std::to_string(mBodyDefCache.position.x) + "  " +
		"y : " + std::to_string(mBodyDefCache.position.y)
	};
	ImGui::TextColored(ImVec4(0.f, 200.f, 0.f, 255), pos.c_str());

	mBodyDefCache.rotation = b2MakeRot(GetOwner()->GetTransform()->GetRotation().z);
	std::string rot = {
		"Rotation : " + std::to_string(b2Rot_GetAngle(mBodyDefCache.rotation))
	};
	ImGui::TextColored(ImVec4(0.f, 200.f, 0.f, 255), rot.c_str());

	ImGui::Separator();
}

void Rigidbody2D::UpdateBodyType()
{
	const char* items[] = { "Static", "Kinematic", "Dynamic" };
	static int currentItem = VALUE_NOT_ASSIGNED;

	switch (mBodyDefCache.type)
	{
	case b2_staticBody:
		currentItem = 0;
		break;
	case b2_kinematicBody:
		currentItem = 1;
		break;
	case b2_dynamicBody:
		currentItem = 2;
		break;
	default:
		break;
	}

	const char* comboPreview = items[currentItem];
	if (ImGui::BeginCombo(ChunkKeys::BODY_TYPE, comboPreview))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (currentItem == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				currentItem = n;
				mBodyDefCache.type = static_cast<b2BodyType>(currentItem);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void Rigidbody2D::UpdateGravityScale()
{
}
#endif // FOXTROT_EDITOR