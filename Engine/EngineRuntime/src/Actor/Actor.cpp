// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor/Actor.h"

#include <fstream>

#include "Actor/ActorData.h"
#include "Scene/Scene.h"
#include "Actor/ActorGroup.h"
#include "Actor/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Component/IComponent.h"
#include "Debugging/DebugMemAlloc.h"
#include "ResourceSystem/FTPremade.h"
#include "Manager/SceneManager.h"
#include "FileSystem/BufferSizes.h"
#include "FileSystem/NullKeys.h"

#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	Actor::Actor(int id)
		: mData(DBG_NEW Common::ActorData)
	{
		mData->Name		  = "New Empty Actor";
		mData->ID		  = id;
		mData->ActorGroup = Common::ActorGroup::DEFAULT;
		mData->State	  = Common::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>;
		mData->Parent	  = nullptr;
		mData->Children	  = DBG_NEW Common::FTDS::DynamicArray<Common::IActor*>;
		mData->DrawOrder  = 0;
	}

	Actor::Actor(Common::IActor* actor, int id)
		: mData(DBG_NEW Common::ActorData)
	{
		mData->Name = "New Copied Actor";
		mData->ID	= id;
		SetActorGroup(actor->GetActorGroup());
		mData->State	  = Common::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>;
		SetParent(actor->GetParent());
		mData->Children	 = DBG_NEW Common::FTDS::DynamicArray<Common::IActor*>;
		mData->DrawOrder = actor->GetData()->DrawOrder;

		mData->Name.Assign(actor->GetNameRef());

		CopyChildObjectFrom(actor);
		CopyTransformFrom(actor);
		CopyComponentsFrom(actor);
	}

	Actor::Actor(Common::IActor* actor, int id, bool deepCpyChild)
		: mData(DBG_NEW Common::ActorData)
	{
		mData->Name = "New Copied Actor";
		mData->ID	= id;
		SetActorGroup(actor->GetActorGroup());
		mData->State	  = Common::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>;
		SetParent(actor->GetParent());
		mData->Children	 = DBG_NEW Common::FTDS::DynamicArray<Common::IActor*>;
		mData->DrawOrder = actor->GetData()->DrawOrder;

		mData->Name.Assign(actor->GetNameRef());

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
		mData->Name += " Copy";
	}

	Actor::~Actor()
	{
		delete mData;
	}

	void Actor::AddChild(Common::IActor* child)
	{
		mData->AddChild(child);
	}

	void Actor::RemoveChild(Common::IActor* child)
	{
		mData->RemoveChild(child);
	}

	void Actor::RemoveComponent(Common::IComponent* component)
	{
		mData->RemoveComponent(component);
	}

	void Actor::RemoveAllComponents()
	{
		mData->RemoveAllComponents();
	}

	void Actor::CopyTransformFrom(Common::IActor* actor)
	{
		mData->CopyTransformFrom(actor);
	}

	void Actor::CopyComponentsFrom(Common::IActor* actor)
	{
		mData->CopyComponentsFrom(actor);
	}

	void Actor::CopyChildObjectFrom(Common::IActor* actor)
	{
		mData->CopyChildObjectFrom<Actor>(actor);
	}

	void Actor::RefChildObjectFrom(Common::IActor* actor)
	{
		mData->RefChildObjectFrom(actor);
	}

	Common::ActorData* Actor::GetData()
	{
		return mData;
	}

	Common::ActorGroup Actor::GetActorGroup() const
	{
		return mData->ActorGroup;
	}

	Common::ActorGroup& Actor::GetActorGroupRef()
	{
		return mData->ActorGroup;
	}

	Common::ActorGroup* Actor::GetActorGroupPtr()
	{
		return &mData->ActorGroup;
	}

	Common::FTDS::String Actor::GetName()
	{
		return mData->Name;
	}

	Common::FTDS::String& Actor::GetNameRef()
	{
		return mData->Name;
	}

	const int Actor::GetID() const
	{
		return mData->ID;
	}

	const bool& Actor::GetIsActive() const
	{
		return mData->IsActive;
	}

	bool& Actor::GetIsActiveRef()
	{
		return mData->IsActive;
	}

	Transform* Actor::GetTransform() const
	{
		return mData->Transform;
	}

	Actor* Actor::GetParent() const
	{
		return reinterpret_cast<Actor*>(mData->Parent);
	}

	Common::FTDS::DynamicArray<Common::IComponent*>* Actor::GetComponents()
	{
		return mData->Components;
	}

	Common::FTDS::DynamicArray<Common::IActor*>* Actor::GetChildActors()
	{
		return mData->Children;
	}

	const int& Actor::GetDrawOrder() const
	{
		return mData->DrawOrder;
	}

	void Actor::SetName(Common::FTDS::String&& name)
	{
		mData->Name.Assign(name);
	}

	void Actor::SetIsActive(bool isActive)
	{
		mData->IsActive = isActive;
	}

	void Actor::SetActorGroup(Common::ActorGroup group)
	{
		mData->ActorGroup = group;
	}

	void Actor::SetState(Common::ActorState state)
	{
		mData->State = state;
	}

	void Actor::SetParent(Common::IActor* parent)
	{
		mData->Parent = parent;
		parent->AddChild(this);
	}

	void Actor::SetTransform(Transform* transform)
	{
		mData->Transform = transform;
	}

	void Actor::SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components)
	{
		mData->Components = components;
	}

	void Actor::SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children)
	{
		mData->Children = children;
	}

	void Actor::SetDrawOrder(int order)
	{
		mData->DrawOrder = order;
	}

	bool Actor::HasName(Common::FTDS::String&& name)
	{
		return mData->Name.Equal(name.C_Str());
	}

	bool Actor::HasName(const char* name)
	{
		return Common::FTDS::StringEqual(mData->Name.C_Str(), name);
	}

	bool Actor::IsDead()
	{
		return mData->State == Common::ActorState::DEAD;
	}

	void Actor::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	void Actor::SaveComponents(std::ofstream& ofs)
	{
		mData->SaveComponents(ofs);
	}

	void Actor::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, Common::ChunkKey::ACTOR_PROPERTIES);

		// Load dummy child Actors which only stores their names.
		size_t childCount = Common::FileIOHelper::BeginDataPackLoad(ifs, Common::ChunkKey::CHILD).first;

		if (0 < childCount)
		{
			for (size_t i = 0; i < childCount; ++i)
			{
				int id = Common::ChunkKey::ID::INVALID;
				Common::FileIOHelper::LoadInt(ifs, id);

				Common::IActor* pending = DBG_NEW Actor(id);
				AddChild(pending);
			}
		}

		// Load dummmy parent Actors which only stores their names.
		Common::FTDS::String parentID;
		Common::FileIOHelper::LoadBasicString(ifs, parentID);
		if (parentID.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
		{
			int	   id	   = std::stoi(parentID.C_Str());
			Actor* pending = DBG_NEW Actor(id);
			SetParent(pending);
		}

		mData->LoadProperties(ifs);
	}

	void Actor::LoadComponents(std::ifstream& ifs)
	{
		mData->LoadComponents(ifs);
	}
} // namespace Core