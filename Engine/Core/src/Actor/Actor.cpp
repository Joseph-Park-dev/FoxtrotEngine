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
		: mName("New Empty Actor")
		, mID(id)
		, mActorGroup(ActorGroup::DEFAULT)
		, mState(ActorState::ALIVE)
		, mIsActive(true)
		, mTransform(DBG_NEW Transform(this))
		, mComponents(DBG_NEW Common::FTDS::DynamicArray<IComponent*>())
		, mParent(nullptr)
		, mChild(DBG_NEW Common::FTDS::DynamicArray<Actor*>())
		, mDrawOrder(0)
	{
		// This block should remain as empty.
	}

	Actor::Actor(Actor* actor, int id)
		: mName("New Copied Actor")
		, mID(id)
		, mActorGroup(actor->mActorGroup)
		, mState(ActorState::ALIVE)
		, mIsActive(true)
		, mTransform(DBG_NEW Transform(this))
		, mComponents(DBG_NEW Common::FTDS::DynamicArray<IComponent*>())
		, mParent(actor->mParent)
		, mChild(DBG_NEW Common::FTDS::DynamicArray<Actor*>())
		, mDrawOrder(actor->mDrawOrder)
	{
		mName.Assign(actor->GetNameRef());

		CopyChildObjectFrom(actor);
		CopyTransformFrom(actor);
		CopyComponentsFrom(actor);
	}

	Actor::Actor(Actor* actor, int id, bool deepCpyChild)
		: mName("New Copied Actor")
		, mID(id)
		, mActorGroup(actor->mActorGroup)
		, mState(ActorState::ALIVE)
		, mIsActive(true)
		, mTransform(DBG_NEW Transform(this))
		, mComponents(DBG_NEW Common::FTDS::DynamicArray<IComponent*>())
		, mParent(actor->mParent)
		, mChild(DBG_NEW Common::FTDS::DynamicArray<Actor*>())
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

		for (size_t i = 0; i < mComponents->Size(); ++i)
		{
			delete mComponents->At(i);
			mComponents->At(i) = nullptr;
		}
		delete mComponents;

		for (size_t i = 0; i < mChild->Size(); ++i)
		{
			delete mChild->At(i);
			mChild->At(i) = nullptr;
		}
		delete mChild;

		mParent = nullptr;
	}

	void Actor::CopyTransformFrom(Actor* actor)
	{
		actor->GetTransform()->CloneTo(mTransform);
	}

	void Actor::CopyComponentsFrom(Actor* actor)
	{
		this->RemoveAllComponents();

		Common::FTDS::DynamicArray<IComponent*>* compsToCopy = actor->GetComponents();
		for (size_t i = 0; i < compsToCopy->GetSize(); ++i)
			compsToCopy->At(i)->CloneTo(this);
	}

	void Actor::CopyChildObjectFrom(Actor* actor)
	{
		if (GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](Actor* child) {
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
		if (actor->GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](Actor* child) {
			this->AddChild(child);
		});
	}

	Common::FTDS::String Actor::GetName()
	{
		return mName;
	}

	Common::FTDS::String& Actor::GetNameRef()
	{
		return mName;
	}

	void Actor::SetName(Common::FTDS::String&& name)
	{
		mName.Assign(name);
	}

	void Actor::AddChild(Actor* child)
	{
		child->mParent = this;
		mChild->PushBack(child);
	}

	void Actor::RemoveChild(Actor* child)
	{
		int pos = mChild->Find(child);
		if (pos == -1)
			return;

		mChild->Erase(pos);

		if (child->mParent->mParent)
			child->mParent = child->mParent->mParent;
	}

	void Actor::AddComponent(IComponent* component)
	{
		int	   updateOrder = component->GetUpdateOrder();
		auto   iter		   = mComponents->Begin();
		size_t iterPos	   = 0;
		for (; iter != mComponents->End(); ++iter)
		{
			if (!(*iter))
				break;

			if (updateOrder < (*iter)->GetUpdateOrder())
				break;
			++iterPos;
		}
		mComponents->Insert(iterPos, component);
	}

	void Actor::RemoveComponent(IComponent* component)
	{
		int pos = mComponents->Find(component);
		if (pos == -1)
			return;

		delete component;
		component = nullptr;
		mComponents->Erase(pos);
	}

	void Actor::RemoveAllComponents()
	{
		for (auto comp = mComponents->Begin(); comp != mComponents->End(); ++comp)
			delete *comp;
		mComponents->Clear();
	}

	bool Actor::HasName(Common::FTDS::String&& name)
	{
		return mName.Equal(name.C_Str());
	}

	bool Actor::HasName(const char* name)
	{
		return Common::FTDS::StringEqual(mName.C_Str(), name);
	}

	bool Actor::IsDead()
	{
		return mState == ActorState::DEAD;
	}

	bool& Actor::IsActive()
	{
		return mIsActive;
	}

	void Actor::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);

		Common::FileIOHelper::SaveString(ofs, ChunkKey::NAME, GetNameRef());
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::ID::ID, mID);
		mTransform->SaveProperties(ofs);
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::DRAW_ORDER, mDrawOrder);
		Common::FileIOHelper::SaveString(ofs, ChunkKey::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::STATE, mIsActive);

		if (mParent)
			Common::FileIOHelper::SaveInt(ofs, ChunkKey::PARENT, mParent->GetID());
		else
			Common::FileIOHelper::SaveString(ofs, ChunkKey::PARENT, Common::ChunkKey::NullVal::NULL_OBJECT);

		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHILD);

		if (0 < mChild->GetSize())
		{
			for (size_t i = 0; i < mChild->GetSize(); ++i)
				Common::FileIOHelper::SaveInt(ofs, std::to_string(i).c_str(), mChild->At(i)->GetID());
		}

		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHILD);

		// Changing the call location of Transform is NOT recommended
		// Nested .chunk DataPack has unknown problem.
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);
	}

	void Actor::SaveComponents(std::ofstream& ofs)
	{
		size_t count = mComponents->GetSize();
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::COMPONENTS);
		for (size_t i = 0; i < count; ++i)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, mComponents->At(i)->GetName());
			mComponents->At(i)->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, mComponents->At(i)->GetName());
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
		Common::FileIOHelper::LoadBool(ifs, mIsActive);

		// Load Actor group
		Common::FTDS::String actorGroupStr;
		Common::FileIOHelper::LoadBasicString(ifs, actorGroupStr);
		mActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

		// Load Actor draw order
		Common::FileIOHelper::LoadInt(ifs, mDrawOrder);

		// Load Transform
		mTransform->LoadProperties(ifs);

		// Load int
		Common::FileIOHelper::LoadInt(ifs, mID);

		// Load Actor name
		Common::FileIOHelper::LoadBasicString(ifs, mName);
	}

	void Actor::LoadComponents(std::ifstream& ifs)
	{
		std::pair<size_t, Common::FTDS::String>&& pack = Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::COMPONENTS);
		mComponents->Reserve(pack.first);
		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String> compPack = Common::FileIOHelper::BeginDataPackLoad(ifs);
			// ChunkLoader::GetInstance()->GetComponentLoadMap().At(compPack.second)->Value()(this, ifs);
		}
	}
} // namespace Core