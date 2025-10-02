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
#include "Dynamic/DynamicArray.h"
#include "Static/FTString.h"
#include "Utils/UUIDGenerator.h"

#ifdef FOXTROT_EDITOR
	#include "EditorElement.h"
	#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

Actor::Actor(int id)
	: mName("New Empty Actor")
	, mID(id)
	, mActorGroup(ActorGroup::DEFAULT)
	, mState(State::EActive)
	, mTransform(DBG_NEW Transform(this))
	, mComponents()
	, mParent(nullptr)
	, mChild()
	, mDrawOrder(0)
{
	// This block should remain as empty.
}

Actor::Actor(Actor* actor, int id)
	: mName("New Copied Actor")
	, mID(id)
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
	if (actor->mParent)
		SetParent(actor->mParent);
	CopyTransformFrom(actor);
	CopyComponentsFrom(actor);
}

Actor::Actor(Actor* actor, int id, bool deepCpyChild)
	: mName("New Copied Actor")
	, mID(id)
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

Actor::Actor(FTPremade* premade, int id)
	: Actor(premade->GetOrigin(), id)
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

	for (size_t i = 0; i < mComponents.Size(); ++i)
	{
		delete mComponents[i];
		mComponents[i] = nullptr;
	}
	mComponents.Clear();

	mChild.Clear();
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

	FTDS::DynamicArray<Component*>& compsToCopy = actor->GetComponents();
	for (size_t i = 0; i < compsToCopy.GetSize(); ++i)
		compsToCopy[i]->CloneTo(this);
}

void Actor::CopyChildObjectFrom(Actor* actor)
{
	if (GetChildActors().GetSize() < 1)
		return;

	actor->GetChildActors().IterateArray([&](Actor* child) {
		if (child)
		{
			ChunkLoader::GetInstance()->AddMaxActorID();
			int maxID = ChunkLoader::GetInstance()->GetMaxActorID();
			this->AddChild(DBG_NEW Actor(child, maxID));
		}
	});
}

void Actor::RefChildObjectFrom(Actor* actor)
{
	if (actor->GetChildActors().GetSize() < 1)
		return;

	actor->GetChildActors().IterateArray([&](Actor* child) {
		this->AddChild(child);
	});
}

void Actor::Initialize(FTCore* coreInst)
{
	if (mComponents.GetSize() < 1)
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
		if (!(*comp)->GetIsInitialized())
			(*comp)->Initialize(coreInst);
}

void Actor::Setup()
{
	if (mComponents.GetSize() < 1)
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
		if (!(*comp)->GetIsSetup())
			(*comp)->Setup();
}

void Actor::ProcessInput(FTInputDevice* inputDevice)
{
	if (!IsActive())
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
	{
		if ((*comp)->GetIsActive())
			(*comp)->ProcessInput(inputDevice);
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	if (!IsActive())
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
	{
		if ((*comp)->GetIsActive())
			(*comp)->Update(deltaTime);
	}
}

void Actor::LateUpdateComponents(float deltaTime)
{
	if (!IsActive())
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
	{
		if (*comp)
		{
			if ((*comp)->GetIsActive())
				(*comp)->LateUpdate(deltaTime);
		}
	}
}

void Actor::RenderComponents(FoxtrotRenderer* renderer)
{
	if (!IsActive())
		return;

	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
	{
		if (*comp)
		{
			if ((*comp)->GetIsActive())
				(*comp)->Render(renderer);
		}
	}
}

void Actor::AddChild(Actor* child)
{
	child->SetParent(this);
	mChild.PushBack(child);
}

void Actor::RemoveChild(Actor* child)
{
	int pos = mChild.Find(child);
	if (pos == -1)
		return;

	mChild.Erase(pos);

	if (child->mParent->mParent)
		child->SetParent(child->mParent->mParent);
}

void Actor::AddComponent(Component* component)
{
	int	 updateOrder = component->GetUpdateOrder();
	auto iter		 = mComponents.Begin();
	for (; iter != mComponents.End(); ++iter)
	{
		if (!(*iter))
			break;

		if (updateOrder < (*iter)->GetUpdateOrder())
			break;
	}
	size_t i = iter.IterPos();
	mComponents.Insert(iter.IterPos(), component);
}

void Actor::RemoveComponent(Component* component)
{
	int pos = mComponents.Find(component);
	if (pos == -1)
		return;

	mComponents.Erase(pos);
	delete component;
	component = nullptr;
}

void Actor::RemoveAllComponents()
{
	for (auto comp = mComponents.Begin(); comp != mComponents.End(); ++comp)
		delete *comp;
	mComponents.Clear();
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
	FileIOHelper::SaveInt(ofs, ChunkKey::ID::ID, mID);
	mTransform->SaveProperties(ofs);
	FileIOHelper::SaveInt(ofs, ChunkKey::DRAW_ORDER, mDrawOrder);
	FileIOHelper::SaveString(ofs, ChunkKey::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::SaveInt(ofs, ChunkKey::STATE, mState);

	if (mParent)
		FileIOHelper::SaveInt(ofs, ChunkKey::PARENT, mParent->GetID());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::PARENT, ChunkKey::NullVal::NULL_OBJECT);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHILD);

	if (0 < mChild.GetSize())
	{
		for (size_t i = 0; i < mChild.GetSize(); ++i)
			FileIOHelper::SaveInt(ofs, std::to_string(i).c_str(), mChild.At(i)->GetID());
	}

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHILD);

	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.GetSize();
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

	if (0 < childCount)
	{
		for (size_t i = 0; i < childCount; ++i)
		{
			int id = ChunkKey::ID::INVALID;
			FileIOHelper::LoadInt(ifs, id);

			Actor* pending = DBG_NEW Actor(id);
			AddChild(pending);
		}
	}

	// Load dummmy parent Actors which only stores their names.
	FTDS::String parentID;
	FileIOHelper::LoadBasicString(ifs, parentID);
	if (parentID.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		int	   id	   = std::stoi(parentID.C_Str());
		Actor* pending = DBG_NEW Actor(id);
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

	// Load int
	FileIOHelper::LoadInt(ifs, mID);

	// Load Actor name
	FileIOHelper::LoadBasicString(ifs, mName);
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::COMPONENTS);
	mComponents.Reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		std::pair<size_t, FTDS::String> compPack = FileIOHelper::BeginDataPackLoad(ifs);
		ChunkLoader::GetInstance()->GetComponentLoadMap().At(compPack.second)->Value()(this, ifs);
	}
}