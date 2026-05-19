// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor/Actor.h"

#include <fstream>

#include "Scene/Scene.h"
#include "InputSystem/IInputDevice.h"
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

#ifdef FOXTROT_EDITOR
	#include "EditorElement.h"
	#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	Actor::Actor(int id)
		: mData(DBG_NEW ActorData)
	{
		mData->Name		  = "New Empty Actor";
		mData->ID		  = id;
		mData->ActorGroup = ActorGroup::DEFAULT;
		mData->State	  = ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<IComponent*>;
		mData->Parent	  = nullptr;
		mData->Children	  = DBG_NEW Common::FTDS::DynamicArray<IActor*>;
		mData->DrawOrder  = 0;
	}

	Actor::Actor(Actor* actor, int id)
		: mData(DBG_NEW ActorData)
	{
		mData->Name		  = "New Copied Actor";
		mData->ID		  = id;
		mData->ActorGroup = actor->mData->ActorGroup;
		mData->State	  = ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<IComponent*>;
		mData->Parent	  = actor->mData->Parent;
		mData->Children	  = DBG_NEW Common::FTDS::DynamicArray<IActor*>;
		mData->DrawOrder  = actor->mData->DrawOrder;

		mData->Name.Assign(actor->GetNameRef());

		CopyChildObjectFrom(actor);
		CopyTransformFrom(actor);
		CopyComponentsFrom(actor);
	}

	Actor::Actor(Actor* actor, int id, bool deepCpyChild)
		: mData(DBG_NEW ActorData)
	{
		mData->Name		  = "New Copied Actor";
		mData->ID		  = id;
		mData->ActorGroup = actor->mData->ActorGroup;
		mData->State	  = ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<IComponent*>;
		mData->Parent	  = actor->mData->Parent;
		mData->Children	  = DBG_NEW Common::FTDS::DynamicArray<IActor*>;
		mData->DrawOrder  = actor->mData->DrawOrder;

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
		if (mData->Transform)
		{
			delete mData->Transform;
			mData->Transform = nullptr;
		}

		for (size_t i = 0; i < mData->Components->Size(); ++i)
		{
			delete mData->Components->At(i);
			mData->Components->At(i) = nullptr;
		}
		delete mData->Components;

		for (size_t i = 0; i < mData->Children->Size(); ++i)
		{
			delete mData->Children->At(i);
			mData->Children->At(i) = nullptr;
		}
		delete mData->Children;

		mData->Parent = nullptr;
	}

	void Actor::CopyTransformFrom(IActor* actor)
	{
		actor->GetTransform()->CloneTo(mData->Transform);
	}

	void Actor::CopyComponentsFrom(IActor* actor)
	{
		this->RemoveAllComponents();

		Common::FTDS::DynamicArray<IComponent*>* compsToCopy = actor->GetComponents();
		for (size_t i = 0; i < compsToCopy->GetSize(); ++i)
			compsToCopy->At(i)->CloneTo(this);
	}

	void Actor::CopyChildObjectFrom(IActor* actor)
	{
		if (GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](IActor* child) {
			if (child)
			{
				Actor* childAc = reinterpret_cast<Actor*>(child);
				ChunkLoader::GetInstance()->AddMaxActorID();
				int maxID = ChunkLoader::GetInstance()->GetMaxActorID();
				this->AddChild(DBG_NEW Actor(childAc, maxID));
			}
		});
	}

	void Actor::RefChildObjectFrom(IActor* actor)
	{
		if (actor->GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](IActor* child) {
			this->AddChild(child);
		});
	}

	ActorGroup Actor::GetActorGroup() const
	{
		return mData->ActorGroup;
	}

	ActorGroup& Actor::GetActorGroupRef()
	{
		return mData->ActorGroup;
	}

	ActorGroup* Actor::GetActorGroupPtr()
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

	Transform* Actor::GetTransform() const
	{
		return mData->Transform;
	}

	Actor* Actor::GetParent() const
	{
		return reinterpret_cast<Actor*>(mData->Parent);
	}

	Common::FTDS::DynamicArray<IComponent*>* Actor::GetComponents()
	{
		return mData->Components;
	}

	Common::FTDS::DynamicArray<IActor*>* Actor::GetChildActors()
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

	void Actor::SetActorGroup(ActorGroup group)
	{
		mData->ActorGroup = group;
	}

	void Actor::SetState(ActorState state)
	{
		mData->State = state;
	}

	void Actor::SetParent(IActor* parent)
	{
		mData->Parent = parent;
		parent->AddChild(this);
	}

	void Actor::SetTransform(Transform* transform)
	{
		mData->Transform = transform;
	}

	void Actor::SetComponents(Common::FTDS::DynamicArray<IComponent*>* components)
	{
		mData->Components = components;
	}

	void Actor::SetChildActors(Common::FTDS::DynamicArray<IActor*>* children)
	{
		mData->Children = children;
	}

	void Actor::SetDrawOrder(int order)
	{
		mData->DrawOrder = order;
	}

	void Actor::AddChild(IActor* child)
	{
		child->SetParent(this);
		mData->Children->PushBack(child);
	}

	void Actor::RemoveChild(IActor* child)
	{
		int pos = mData->Children->Find(child);
		if (pos == -1)
			return;

		mData->Children->Erase(pos);

		if (child->GetParent()->GetParent())
			child->SetParent(child->GetParent()->GetParent());
	}

	void Actor::AddComponent(IComponent* component)
	{
		int	   updateOrder = component->GetUpdateOrder();
		auto   iter		   = mData->Components->Begin();
		size_t iterPos	   = 0;
		for (; iter != mData->Components->End(); ++iter)
		{
			if (!(*iter))
				break;

			if (updateOrder < (*iter)->GetUpdateOrder())
				break;
			++iterPos;
		}
		mData->Components->Insert(iterPos, component);
	}

	void Actor::RemoveComponent(IComponent* component)
	{
		int pos = mData->Components->Find(component);
		if (pos == -1)
			return;

		delete component;
		component = nullptr;
		mData->Components->Erase(pos);
	}

	void Actor::RemoveAllComponents()
	{
		for (auto comp = mData->Components->Begin(); comp != mData->Components->End(); ++comp)
			delete *comp;
		mData->Components->Clear();
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
		return mData->State == ActorState::DEAD;
	}

	bool& Actor::IsActive()
	{
		return mData->IsActive;
	}

	void Actor::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);

		Common::FileIOHelper::SaveString(ofs, ChunkKey::NAME, mData->Name);
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::ID::ID, mData->ID);
		mData->Transform->SaveProperties(ofs);
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::DRAW_ORDER, mData->DrawOrder);
		Common::FileIOHelper::SaveString(ofs, ChunkKey::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mData->ActorGroup));
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::STATE, mData->IsActive);

		if (mData->Parent)
			Common::FileIOHelper::SaveInt(ofs, ChunkKey::PARENT, mData->Parent->GetID());
		else
			Common::FileIOHelper::SaveString(ofs, ChunkKey::PARENT, Common::ChunkKey::NullVal::NULL_OBJECT);

		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHILD);

		if (0 < mData->Children->GetSize())
		{
			for (size_t i = 0; i < mData->Children->GetSize(); ++i)
				Common::FileIOHelper::SaveInt(ofs, std::to_string(i).c_str(), mData->Children->At(i)->GetID());
		}

		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHILD);

		// Changing the call location of Transform is NOT recommended
		// Nested .chunk DataPack has unknown problem.
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);
	}

	void Actor::SaveComponents(std::ofstream& ofs)
	{
		size_t count = mData->Components->GetSize();
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::COMPONENTS);
		for (size_t i = 0; i < count; ++i)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, mData->Components->At(i)->GetName());
			mData->Components->At(i)->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, mData->Components->At(i)->GetName());
		}
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::COMPONENTS);
	}

	void Actor::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_PROPERTIES);

		// Load dummy child Actors which only stores their names.
		size_t childCount = Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHILD).first;

		if (0 < childCount)
		{
			for (size_t i = 0; i < childCount; ++i)
			{
				int id = ChunkKey::ID::INVALID;
				Common::FileIOHelper::LoadInt(ifs, id);

				Actor* pending = DBG_NEW Actor(id);
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

		// Load Actor state
		Common::FileIOHelper::LoadBool(ifs, mData->IsActive);

		// Load Actor group
		Common::FTDS::String actorGroupStr;
		Common::FileIOHelper::LoadBasicString(ifs, actorGroupStr);
		mData->ActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

		// Load Actor draw order
		Common::FileIOHelper::LoadInt(ifs, mData->DrawOrder);

		// Load Transform
		mData->Transform->LoadProperties(ifs);

		// Load int
		Common::FileIOHelper::LoadInt(ifs, mData->ID);

		// Load Actor name
		Common::FileIOHelper::LoadBasicString(ifs, mData->Name);
	}

	void Actor::LoadComponents(std::ifstream& ifs)
	{
		std::pair<size_t, Common::FTDS::String>&& pack = Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::COMPONENTS);
		mData->Components->Reserve(pack.first);
		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String> compPack = Common::FileIOHelper::BeginDataPackLoad(ifs);
			// ChunkLoader::GetInstance()->GetComponentLoadMap().At(compPack.second)->Value()(this, ifs);
		}
	}
} // namespace Core