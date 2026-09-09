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
	/// @brief Initializes an actor's identity, transform, and component/child collections.
	/// @param id Actor or entry identifier.
	/// @note Initializes the :Actor base or delegates to its constructor.
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

	/// @brief Initializes an actor's identity, transform, and component/child collections.
	/// @param actor Actor participating in this operation.
	/// @param id Actor or entry identifier.
	/// @note Initializes the :Actor base or delegates to its constructor.
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

	/// @brief Initializes an actor's identity, transform, and component/child collections.
	/// @param actor Actor participating in this operation.
	/// @param id Actor or entry identifier.
	/// @param deepCpyChild Whether child actors are cloned instead of sharing references.
	/// @note Initializes the :Actor base or delegates to its constructor.
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

	/// @brief Initializes an actor's identity, transform, and component/child collections.
	/// @param premade Premade resource used as an actor template.
	/// @param id Actor or entry identifier.
	/// @note Initializes the :Actor base or delegates to its constructor.
	Actor::Actor(FTPremade* premade, int id)
		: Actor(premade->GetOrigin(), id)
	{
		mData->Name += " Copy";
	}

	/// @brief Releases the resources managed by this instance during destruction.
	Actor::~Actor()
	{
		delete mData;
	}

	/// @brief Registers an actor in this actor's child collection.
	/// @param child Child actor to attach or detach.
	void Actor::AddChild(Common::IActor* child)
	{
		mData->AddChild(child);
	}

	/// @brief Detaches an actor from this actor's child collection.
	/// @param child Child actor to attach or detach.
	void Actor::RemoveChild(Common::IActor* child)
	{
		mData->RemoveChild(child);
	}

	/// @brief Removes the specified component from the actor's component collection.
	/// @param component Component instance associated with the actor or plugin.
	void Actor::RemoveComponent(Common::IComponent* component)
	{
		mData->RemoveComponent(component);
	}

	/// @brief Removes all components associated with the actor.
	void Actor::RemoveAllComponents()
	{
		mData->RemoveAllComponents();
	}

	/// @brief Copies spatial state from the source actor into this actor's transform.
	/// @param actor Actor participating in this operation.
	void Actor::CopyTransformFrom(Common::IActor* actor)
	{
		mData->CopyTransformFrom(actor);
	}

	/// @brief Clones the source actor's components for this actor.
	/// @param actor Actor participating in this operation.
	void Actor::CopyComponentsFrom(Common::IActor* actor)
	{
		mData->CopyComponentsFrom(actor);
	}

	/// @brief Clones the source actor's child hierarchy.
	/// @param actor Actor participating in this operation.
	void Actor::CopyChildObjectFrom(Common::IActor* actor)
	{
		mData->CopyChildObjectFrom<Actor>(actor);
	}

	/// @brief Associates the source actor's children with this editor or runtime actor.
	/// @param actor Actor participating in this operation.
	void Actor::RefChildObjectFrom(Common::IActor* actor)
	{
		mData->RefChildObjectFrom(actor);
	}

	/// @brief Returns the data used by this actor.
	/// @return Borrowed access to the data.
	Common::ActorData* Actor::GetData()
	{
		return mData;
	}

	/// @brief Returns the actor group used by this actor.
	/// @return Current actor group.
	Common::ActorGroup Actor::GetActorGroup() const
	{
		return mData->ActorGroup;
	}

	/// @brief Returns the actor group ref used by this actor.
	/// @return Borrowed access to the actor group ref.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::ActorGroup& Actor::GetActorGroupRef()
	{
		return mData->ActorGroup;
	}

	/// @brief Returns the actor group ptr used by this actor.
	/// @return Borrowed access to the actor group ptr.
	Common::ActorGroup* Actor::GetActorGroupPtr()
	{
		return &mData->ActorGroup;
	}

	/// @brief Returns the name used by this actor.
	/// @return Current name.
	Common::FTDS::String Actor::GetName()
	{
		return mData->Name;
	}

	/// @brief Returns the name ref used by this actor.
	/// @return Borrowed access to the name ref.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::FTDS::String& Actor::GetNameRef()
	{
		return mData->Name;
	}

	/// @brief Returns the id used by this actor.
	/// @return Current id.
	const int Actor::GetID() const
	{
		return mData->ID;
	}

	/// @brief Returns the is active used by this actor.
	/// @return Current value of the is active flag.
	const bool& Actor::GetIsActive() const
	{
		return mData->IsActive;
	}

	/// @brief Returns the is active ref used by this actor.
	/// @return Current value of the is active ref flag.
	bool& Actor::GetIsActiveRef()
	{
		return mData->IsActive;
	}

	/// @brief Returns the transform used by this actor.
	/// @return Borrowed access to the transform.
	Transform* Actor::GetTransform() const
	{
		return mData->Transform;
	}

	/// @brief Returns the parent used by this actor.
	/// @return Borrowed access to the parent.
	Actor* Actor::GetParent() const
	{
		return reinterpret_cast<Actor*>(mData->Parent);
	}

	/// @brief Returns the components used by this actor.
	/// @return Borrowed access to the components.
	Common::FTDS::DynamicArray<Common::IComponent*>* Actor::GetComponents()
	{
		return mData->Components;
	}

	/// @brief Returns the child actors used by this actor.
	/// @return Borrowed access to the child actors.
	Common::FTDS::DynamicArray<Common::IActor*>* Actor::GetChildActors()
	{
		return mData->Children;
	}

	/// @brief Returns the draw order used by this actor.
	/// @return Borrowed access to the draw order.
	const int& Actor::GetDrawOrder() const
	{
		return mData->DrawOrder;
	}

	/// @brief Updates the name used by subsequent operations.
	/// @param name Replacement name.
	void Actor::SetName(Common::FTDS::String&& name)
	{
		mData->Name.Assign(name);
	}

	/// @brief Updates the is active used by subsequent operations.
	/// @param isActive Replacement is active.
	void Actor::SetIsActive(bool isActive)
	{
		mData->IsActive = isActive;
	}

	/// @brief Updates the actor group used by subsequent operations.
	/// @param group Replacement actor group.
	void Actor::SetActorGroup(Common::ActorGroup group)
	{
		mData->ActorGroup = group;
	}

	/// @brief Updates the state used by subsequent operations.
	/// @param state Replacement state.
	void Actor::SetState(Common::ActorState state)
	{
		mData->State = state;
	}

	/// @brief Updates the parent used by subsequent operations.
	/// @param parent Replacement parent.
	void Actor::SetParent(Common::IActor* parent)
	{
		mData->Parent = parent;
		parent->AddChild(this);
	}

	/// @brief Updates the transform used by subsequent operations.
	/// @param transform Replacement transform.
	void Actor::SetTransform(Transform* transform)
	{
		mData->Transform = transform;
	}

	/// @brief Updates the components used by subsequent operations.
	/// @param components Replacement components.
	void Actor::SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components)
	{
		mData->Components = components;
	}

	/// @brief Updates the child actors used by subsequent operations.
	/// @param children Replacement child actors.
	void Actor::SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children)
	{
		mData->Children = children;
	}

	/// @brief Updates the draw order used by subsequent operations.
	/// @param order Replacement draw order.
	void Actor::SetDrawOrder(int order)
	{
		mData->DrawOrder = order;
	}

	/// @brief Compares the actor's stored name with the supplied name.
	/// @param name Name used to identify the requested object or interface.
	/// @return True when the compared values match; otherwise false.
	bool Actor::HasName(Common::FTDS::String&& name)
	{
		return mData->Name.Equal(name.C_Str());
	}

	/// @brief Compares the actor's stored name with the supplied name.
	/// @param name Name used to identify the requested object or interface.
	/// @return True when the compared values match; otherwise false.
	bool Actor::HasName(const char* name)
	{
		return Common::FTDS::StringEqual(mData->Name.C_Str(), name);
	}

	/// @brief Tests whether the actor state requests deferred destruction.
	/// @return True when the actor state requests deferred destruction; otherwise false.
	bool Actor::IsDead()
	{
		return mData->State == Common::ActorState::DEAD;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void Actor::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	/// @brief Serializes the actor's attached components to the .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	void Actor::SaveComponents(std::ofstream& ofs)
	{
		mData->SaveComponents(ofs);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
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

	/// @brief Restores actor components through the available plugin factories.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void Actor::LoadComponents(std::ifstream& ifs)
	{
		mData->LoadComponents(ifs);
	}
} // namespace Core
