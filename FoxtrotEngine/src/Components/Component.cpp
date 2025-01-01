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
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

void Component::Initialize(FTCore* coreInstance)
{
	mIsInitialized = true;
}

void Component::Setup()
{
	mIsSetup = true;
}

void Component::ProcessInput(KeyInputManager* keyInputManager)
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

const int Component::GetDrawOrder() const
{
	return mDrawOrder;
}

const bool Component::GetIsInitialized() const
{
	return mIsInitialized;
}

const bool Component::GetIsSetup() const
{
	return mIsSetup;
}

Component::Component(Actor* owner, int drawOrder, int updateOrder)
	: mOwner(owner)
	, mDrawOrder(drawOrder)
	, mUpdateOrder(updateOrder)
	, mIsInitialized(false)
	, mIsSetup(false)
{
	mOwner->AddComponent(this);
}

Component::Component(const Component* origin)
	: mOwner(origin->mOwner)
	, mUpdateOrder(origin->mUpdateOrder)
	, mDrawOrder(origin->mDrawOrder)
	, mIsInitialized(false)
	, mIsSetup(false)
{
	mOwner->AddComponent(this);
}

void Component::CloneTo(Actor* actor)
{}

Component::~Component()
{
}

void Component::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::SaveInt(ofs, ChunkKeys::DRAW_ORDER, mDrawOrder);
	FileIOHelper::SaveInt(ofs, ChunkKeys::UPDATE_ORDER, mUpdateOrder);
}

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
	FileIOHelper::LoadInt(ifs, mDrawOrder);
}

#ifdef FOXTROT_EDITOR
void Component::EditorUIUpdate()
{
}
void Component::UpdateDebugShapes(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* cameraInst)
{}

void Component::OnConfirmUpdate()
{
}
#endif