// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actors/Actor.h"

#include <algorithm>
#include <functional>

#include "Scenes/Scene.h"
#include "Managers/KeyInputManager.h"

#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Component.h"
#include "Components/Collider2DComponent.h"
#include "Core/FTCore.h"
#include "Debugging/DebugMemAlloc.h"
#include "ResourceSystem/FTPremade.h"

#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

Actor::Actor()
	: mName("New Empty Actor")
	, mActorGroup(ActorGroup::DEFAULT)
	, mState(State::EActive)
	, mTransform(DBG_NEW Transform)
	, mComponents()
	, mParent(nullptr)
	, mChild()
{
	// THIS BLOCK SHOULD BE REMAINED EMPTY;
}

Actor::Actor(Scene* scene)
	: mName("New Empty Actor")
	, mActorGroup(ActorGroup::DEFAULT)
	, mState(EActive)
	, mTransform(DBG_NEW Transform)
	, mComponents()
	, mParent(nullptr)
	, mChild{}
{
	scene->AddActor(this, mActorGroup);
}

Actor::Actor(Actor* actor)
	: mName(actor->GetName())
	, mActorGroup(actor->mActorGroup)
	, mState(EActive)
	, mTransform(DBG_NEW Transform)
	, mComponents()
	, mParent(actor->mParent)
	, mChild{}
{
	CopyTransformFrom(actor);
	CopyComponentsFrom(actor);
	CopyChildObjectFrom(actor);
}

Actor::Actor(Actor* actor, Scene* scene)
	: mName			(actor->GetName())
	, mActorGroup	(actor->mActorGroup)
	, mState		(actor->mState)
	, mTransform	(DBG_NEW Transform)
	, mComponents	()
	, mParent		(nullptr)
	, mChild		()
{
	CopyTransformFrom(actor);
	CopyComponentsFrom(actor);
	CopyChildObjectFrom(actor);

	scene->AddActor(this, mActorGroup);
}

Actor::Actor(FTPremade* premade, Scene* scene)
	: Actor(premade->GetOrigin(), scene)
{
	this->mName += " Copy";
}

Actor::~Actor()
{
	if (mTransform)
	{
		delete mTransform;
		mTransform = nullptr;
	}

	for (size_t i = 0; i < mComponents.size(); ++i)
		delete mComponents[i];
	mComponents.clear();

	for (size_t i = 0; i < mChild.size(); ++i)
		mChild.clear();	
}

void Actor::CopyTransformFrom(Actor* actor)
{
	Transform* originTransf = actor->GetTransform();
	mTransform->SetWorldPosition(originTransf->GetWorldPosition());
	mTransform->SetLocalPosition(originTransf->GetLocalPosition());
	//copied->SetScreenPosition(originTransf->GetScreenPosition());
	mTransform->SetScale(originTransf->GetScale());
	mTransform->SetRotation(originTransf->GetRotation());
}

void Actor::CopyComponentsFrom(Actor* actor)
{
	this->RemoveAllComponents();

	std::vector<Component*>& compsToCopy = actor->GetComponents();
	for (size_t i = 0; i < compsToCopy.size(); ++i)
		compsToCopy[i]->CloneTo(this);
}

void Actor::CopyChildObjectFrom(Actor* actor)
{
	//std::vector<Actor*>& childObjects = actor->GetChildActors();
	//for (size_t i = 0; i < childObjects.size(); ++i)
	//	this->AddChild(DBG_NEW Actor(childObjects[i]));
}

void Actor::Initialize(FTCore* coreInst)
{
	for (size_t i = 0; i < mComponents.size(); ++i)
		if (!mComponents[i]->GetIsInitialized())
			mComponents[i]->Initialize(coreInst);
}

void Actor::Setup()
{
	for (auto comp : mComponents)
		if (!comp->GetIsSetup())
			comp->Setup();
}

void Actor::ProcessInput(KeyInputManager* keyInputManager)
{
	if (IsActive())
		for (auto comp : mComponents)
			comp->ProcessInput(keyInputManager);
}

void Actor::UpdateComponents(float deltaTime)
{
	if (IsActive())
		for (auto comp : mComponents)
			comp->Update(deltaTime);
}

void Actor::LateUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
		comp->LateUpdate(deltaTime);
}

void Actor::RenderComponents(FoxtrotRenderer* renderer)
{
	for (auto comp : mComponents)
		comp->Render(renderer);
}

void Actor::AddComponent(Component* component)
{
	int updateOrder = component->GetUpdateOrder();
	int drawOrder = component->GetDrawOrder();
	auto iter = mComponents.begin();
	for (;iter != mComponents.end();++iter)
	{
		if (updateOrder < (*iter)->GetUpdateOrder())
			break;
	}
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		delete component;
		mComponents.erase(iter);
	}
}

void Actor::RemoveAllComponents()
{
	for (size_t i = 0; i < mComponents.size(); ++i)
		delete mComponents[i];
	mComponents.clear();
}

void Actor::OnCollisionEnter(Collider2DComponent* other)
{
	LogString(mName + " : Collision Enter with " + other->GetOwner()->GetName());
}

void Actor::OnCollisionStay(Collider2DComponent* other)
{
}

void Actor::OnCollisionExit(Collider2DComponent* other)
{
	LogString(mName + " : Collision Exit with " + other->GetOwner()->GetName());
}

void Actor::OnRayEnter()
{
}

std::string Actor::GetStateStr() const
{
	std::string state = "active";
	if (mState == EPaused)
		state = "paused";
	else if (mState == EDead)
		state = "dead";
	return state;
}

void Actor::SetState(std::string state)
{
	if (state == "paused")
		mState = EPaused;
	else if (state == "dead")
		mState = EDead;
	else
		mState = EActive;
}

bool Actor::HasName(std::string& name)
{
	return this->GetName() == name;
}

bool Actor::HasName(const char* name)
{
	return FTDS::StringEqual(mName.c_str(), name);
}

void Actor::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES);
	FileIOHelper::SaveString		(ofs, ChunkKeys::NAME, GetName());
	FileIOHelper::SaveString		(ofs, ChunkKeys::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::SaveString		(ofs, ChunkKeys::STATE, GetStateStr());
	if (mParent)
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, mParent->GetName());
	else
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, "nullptr");

	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	mTransform->SaveProperties		(ofs);
	FileIOHelper::EndDataPackSave	(ofs, ChunkKeys::ACTOR_PROPERTIES);
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.size();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::COMPONENTS);
	for (size_t i = 0; i < count; ++i) {
		FileIOHelper::BeginDataPackSave(ofs, mComponents[i]->GetName());
		mComponents[i]->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, mComponents[i]->GetName());
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::COMPONENTS);
}

void Actor::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::ACTOR_PROPERTIES);
	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	mTransform->LoadProperties(ifs);

	std::string parentName;
	FileIOHelper::LoadBasicString(ifs, parentName);
	// <Parent finding feature here!>

	std::string stateStr;
	FileIOHelper::LoadBasicString(ifs, stateStr);
	SetState(stateStr);

	std::string actorGroupStr;
	FileIOHelper::LoadBasicString(ifs, actorGroupStr);
	mActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

	FileIOHelper::LoadBasicString(ifs, mName);
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	std::pair<int, std::string>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::COMPONENTS);
	mComponents.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i) {
		std::pair<size_t, std::string> compPack = FileIOHelper::BeginDataPackLoad(ifs);
		ChunkLoader::GetInstance()->GetComponentLoadMap().at(compPack.second)(this, ifs);
	}
}