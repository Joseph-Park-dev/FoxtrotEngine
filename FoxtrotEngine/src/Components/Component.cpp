// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component.h"

#include "Actors/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "Components/Collider2D.h"

void Component::Initialize(FTCore* coreInstance)
{
	mIsInitialized = true;
}

void Component::Setup()
{
	mIsSetup = true;
}

void Component::ProcessInput(FTInputDevice* inputDevice)
{
}

void Component::Update(float deltaTime)
{
}

void Component::LateUpdate(float deltaTime)
{
}

void Component::Render(FoxtrotRenderer* renderer)
{
}

Actor* Component::GetOwner() const
{
	return mOwner;
}

const int Component::GetUpdateOrder() const
{
	return mUpdateOrder;
}

const bool Component::GetIsInitialized() const
{
	return mIsInitialized;
}

const bool Component::GetIsSetup() const
{
	return mIsSetup;
}

const bool Component::GetIsActive() const
{
	return mIsActive;
}

void Component::SetIsActive(bool isActive)
{
	mIsActive = isActive;
}

Component::Component(Actor* owner, int updateOrder)
	: mOwner(owner)
	, mUpdateOrder(updateOrder)
	, mIsInitialized(false)
	, mIsSetup(false)
	, mIsActive(true)
{
	mOwner->AddComponent(this);
}

Component::Component(const Component* origin)
	: mOwner(origin->mOwner)
	, mUpdateOrder(origin->mUpdateOrder)
	, mIsInitialized(false)
	, mIsSetup(false)
	, mIsActive(origin->mIsActive)
{
	mOwner->AddComponent(this);
}

Component::~Component()
{
}

void Component::Copy(Component* to)
{
	to->mOwner = this->mOwner;
	to->mUpdateOrder = this->mUpdateOrder;
}

void Component::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::SaveBool(ofs, ChunkKey::IS_ACTIVE, mIsActive);
	FileIOHelper::SaveInt(ofs, ChunkKey::UPDATE_ORDER, mUpdateOrder);
}

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
	FileIOHelper::LoadBool(ifs, mIsActive);
}

#ifdef FOXTROT_EDITOR
void Component::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateBoolValue("Is Active", mIsActive);
}

void Component::SetUpdateOrder(int updateOrder)
{
	mUpdateOrder = updateOrder;
}
#endif