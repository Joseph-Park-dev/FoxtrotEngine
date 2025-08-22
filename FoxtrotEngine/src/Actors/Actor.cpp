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
#include "InputSystem/FTInputDevice.h"

#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Component.h"
#include "Components/Collider2D.h"
#include "Core/FTCore.h"
#include "Debugging/DebugMemAlloc.h"
#include "ResourceSystem/FTPremade.h"
#include "Managers/SceneManager.h"
#include "FileSystem/BufferSizes.h"

#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "EditorElement.h"
	#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

Actor::Actor()
	: mName("New Empty Actor")
	, mActorGroup(ActorGroup::DEFAULT)
	, mState(State::EActive)
	, mTransform(DBG_NEW Transform(this))
	, mComponents()
	, mParent(nullptr)
	, mChild()
	, mDrawOrder(0)
{
	// THIS BLOCK SHOULD BE REMAINED EMPTY;
}

Actor::Actor(Actor* actor)
	: mName("New Copied Actor")
	, mActorGroup(actor->mActorGroup)
	, mState(EActive)
	, mTransform(DBG_NEW Transform(this))
	, mComponents()
	, mParent(actor->mParent)
	, mChild{}
	, mDrawOrder(actor->mDrawOrder)
{
	mName.Assign(actor->GetNameRef());

	CopyChildObjectFrom(actor);
	CopyTransformFrom(actor);
	CopyComponentsFrom(actor);
}

Actor::Actor(Actor* actor, bool deepCpyChild)
	: mName("New Copied Actor")
	, mActorGroup(actor->mActorGroup)
	, mState(EActive)
	, mTransform(DBG_NEW Transform(this))
	, mComponents()
	, mParent(actor->mParent)
	, mChild{}
	, mDrawOrder(actor->mDrawOrder)
{
	mName.Assign(actor->GetNameRef());

	if (deepCpyChild)
		CopyChildObjectFrom(actor);
	else
		RefChildObjectFrom(actor);

	CopyTransformFrom(actor);
	CopyComponentsFrom(actor);
}

Actor::Actor(FTPremade* premade)
	: Actor(premade->GetOrigin())
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

	mChild.clear();
	mParent = nullptr;
}

void Actor::CopyTransformFrom(Actor* actor)
{
	Transform* originTransf = actor->GetTransform();
	actor->GetTransform()->CloneTo(mTransform);
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
	for (Actor* child : actor->GetChildActors())
		this->AddChild(DBG_NEW Actor(child));
}

void Actor::RefChildObjectFrom(Actor* actor)
{
	for (Actor* child : actor->GetChildActors())
		this->AddChild(child);
}

void Actor::Initialize(FTCore* coreInst)
{
	for (Actor* pending : mChild)
	{
		Actor* child = FIND_ACTOR(pending->GetNameRef(), pending);

		// Distinguish if the Actor is a valid pointer.
		RemoveChild(pending);
		delete pending;
		pending = nullptr;
		this->AddChild(child);  // This also adds this object as the parent to child.
	}

	mTransform->SetOwner(this);

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

void Actor::ProcessInput(FTInputDevice* inputDevice)
{
	if (IsActive())
		for (auto comp : mComponents)
			if (comp->GetIsActive())
				comp->ProcessInput(inputDevice);
}

void Actor::UpdateComponents(float deltaTime)
{
	if (IsActive())
		for (auto comp : mComponents)
			if (comp->GetIsActive())
				comp->Update(deltaTime);
}

void Actor::LateUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
		if (comp->GetIsActive())
			comp->LateUpdate(deltaTime);
}

void Actor::RenderComponents(FoxtrotRenderer* renderer)
{
	for (auto comp : mComponents)
		if (comp->GetIsActive())
			comp->Render(renderer);
}

void Actor::AddChild(Actor* child)
{
	child->SetParent(this);
	mChild.emplace_back(child);
}

void Actor::RemoveChild(Actor* child)
{
	auto iter = std::find(mChild.begin(), mChild.end(), child);
	if (iter != mChild.end())
		mChild.erase(iter);
	child->SetParent(child->mParent->mParent);
}

void Actor::AddComponent(Component* component)
{
	int	 updateOrder = component->GetUpdateOrder();
	auto iter		 = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
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

FTDS::String Actor::GetStateStr() const
{
	FTDS::String state = "active";
	if (mState == EPaused)
		state = "paused";
	else if (mState == EDead)
		state = "dead";
	return state;
}

void Actor::SetState(FTDS::String state)
{
	if (state == "paused")
		mState = EPaused;
	else if (state == "dead")
		mState = EDead;
	else
		mState = EActive;
}

bool Actor::HasName(FTDS::String& name)
{
	return mName.Equal(name.C_Str());
}

bool Actor::HasName(const char* name)
{
	return FTDS::StringEqual(mName.C_Str(), name);
}

void Actor::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);

	FileIOHelper::SaveString(ofs, ChunkKey::NAME, GetNameRef());
	mTransform->SaveProperties(ofs);
	FileIOHelper::SaveInt(ofs, ChunkKey::DRAW_ORDER, mDrawOrder);
	FileIOHelper::SaveString(ofs, ChunkKey::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::SaveInt(ofs, ChunkKey::STATE, mState);

	if (mParent)
		FileIOHelper::SaveString(ofs, ChunkKey::PARENT, mParent->GetNameRef());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::PARENT, ChunkKey::NullVal::NULL_OBJECT);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHILD);
	for (size_t i = 0; i < mChild.size(); ++i)
		FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), mChild.at(i)->GetNameRef());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHILD);

	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.size();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::COMPONENTS);
	for (size_t i = 0; i < count; ++i)
	{
		FileIOHelper::BeginDataPackSave(ofs, mComponents[i]->GetName());
		mComponents[i]->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, mComponents[i]->GetName());
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::COMPONENTS);
}

void Actor::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_PROPERTIES);

	// Load dummy child Actors which only stores their names.
	size_t childCount = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHILD).first;
	for (size_t i = 0; i < childCount; ++i)
	{
		FTDS::String childName;
		FileIOHelper::LoadBasicString(ifs, childName);
		if (childName.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		{
			Actor* pending = DBG_NEW Actor;
			pending->SetName(childName);
			AddChild(pending);
		}
	}

	// Load dummmy parent Actors which only stores their names.
	FTDS::String parentName;
	FileIOHelper::LoadBasicString(ifs, parentName);
	if (parentName.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		Actor* pending = DBG_NEW Actor;
		pending->SetName(parentName);
		SetParent(pending);
	}

	// Load Actor state
	int stateInt = 0;
	FileIOHelper::LoadInt(ifs, stateInt);
	SetState(static_cast<State>(stateInt));

	// Load Actor group
	FTDS::String actorGroupStr;
	FileIOHelper::LoadBasicString(ifs, actorGroupStr);
	mActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

	// Load Actor draw order
	FileIOHelper::LoadInt(ifs, mDrawOrder);

	// Load Transform
	mTransform->LoadProperties(ifs);

	// Load Actor name
	FileIOHelper::LoadBasicString(ifs, mName);
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::COMPONENTS);
	mComponents.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		std::pair<size_t, FTDS::String> compPack = FileIOHelper::BeginDataPackLoad(ifs);
		ChunkLoader::GetInstance()->GetComponentLoadMap().At(compPack.second)->Value()(this, ifs);
	}
}