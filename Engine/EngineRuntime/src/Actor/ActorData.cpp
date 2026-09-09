#include "Actor/ActorData.h"

#include "Actor/IActor.h"
#include "Actor/ActorGroup.h"
#include "FileSystem/FileIOHelper.h"
#include "Actor/Transform.h"

namespace Common
{
	/// @brief Registers an actor in this actor's child collection.
	/// @param child Child actor to attach or detach.
	void Common::ActorData::AddChild(Common::IActor* child)
	{
		child->SetParent(Owner);
		Children->PushBack(child);
	}

	/// @brief Detaches an actor from this actor's child collection.
	/// @param child Child actor to attach or detach.
	void Common::ActorData::RemoveChild(Common::IActor* child)
	{
		int pos = Children->Find(child);
		if (pos == -1)
			return;

		Children->Erase(pos);

		// if (child->GetParent()->GetParent())
		//	child->SetParent(child->GetParent()->GetParent());
	}

	/// @brief Removes the specified component from the actor's component collection.
	/// @param comp Component instance associated with the actor or plugin.
	void Common::ActorData::RemoveComponent(Common::IComponent* comp)
	{
		int pos = Components->Find(comp);
		if (pos == -1)
			return;

		delete comp;
		comp = nullptr;
		Components->Erase(pos);
	}

	/// @brief Removes all components associated with the actor.
	void Common::ActorData::RemoveAllComponents()
	{
		for (auto comp = Components->Begin(); comp != Components->End(); ++comp)
			delete *comp;
		Components->Clear();
	}

	/// @brief Copies spatial state from the source actor into this actor's transform.
	/// @param actor Actor participating in this operation.
	void Common::ActorData::CopyTransformFrom(Common::IActor* actor)
	{
		actor->GetTransform()->CloneTo(Transform);
	}

	/// @brief Clones the source actor's components for this actor.
	/// @param actor Actor participating in this operation.
	void Common::ActorData::CopyComponentsFrom(Common::IActor* actor)
	{
		this->RemoveAllComponents();

		Common::FTDS::DynamicArray<IComponent*>* compsToCopy = actor->GetComponents();
		for (size_t i = 0; i < compsToCopy->GetSize(); ++i)
			compsToCopy->At(i)->CloneTo(Owner);
	}

	/// @brief Associates the source actor's children with this editor or runtime actor.
	/// @param actor Actor participating in this operation.
	void Common::ActorData::RefChildObjectFrom(Common::IActor* actor)
	{
		if (actor->GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](Common::IActor* child) {
			this->AddChild(child);
		});
	}

	/// @brief Returns the actor group used by this actor data.
	/// @return Current actor group.
	Common::ActorGroup Common::ActorData::GetActorGroup() const
	{
		return ActorGroup;
	}

	/// @brief Returns the actor group ref used by this actor data.
	/// @return Borrowed access to the actor group ref.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::ActorGroup& Common::ActorData::GetActorGroupRef()
	{
		return ActorGroup;
	}

	/// @brief Returns the actor group ptr used by this actor data.
	/// @return Borrowed access to the actor group ptr.
	Common::ActorGroup* Common::ActorData::GetActorGroupPtr()
	{
		return &ActorGroup;
	}

	/// @brief Returns the name used by this actor data.
	/// @return Current name.
	Common::FTDS::String Common::ActorData::GetName()
	{
		return Name;
	}

	/// @brief Returns the name ref used by this actor data.
	/// @return Borrowed access to the name ref.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::FTDS::String& Common::ActorData::GetNameRef()
	{
		return Name;
	}

	/// @brief Returns the id used by this actor data.
	/// @return Current id.
	const int Common::ActorData::GetID() const
	{
		return ID;
	}

	/// @brief Returns the is active used by this actor data.
	/// @return Current value of the is active flag.
	const bool Common::ActorData::GetIsActive() const
	{
		return IsActive;
	}

	/// @brief Returns the is active ref used by this actor data.
	/// @return Current value of the is active ref flag.
	bool& Common::ActorData::GetIsActiveRef()
	{
		return IsActive;
	}

	/// @brief Returns the transform used by this actor data.
	/// @return Borrowed access to the transform.
	Core::Transform* Common::ActorData::GetTransform() const
	{
		return Transform;
	}

	/// @brief Returns the parent used by this actor data.
	/// @return Borrowed access to the parent.
	Common::IActor* Common::ActorData::GetParent() const
	{
		return Parent;
	}

	/// @brief Returns the components used by this actor data.
	/// @return Borrowed access to the components.
	Common::FTDS::DynamicArray<IComponent*>* Common::ActorData::GetComponents()
	{
		return Components;
	}

	/// @brief Returns the child actors used by this actor data.
	/// @return Borrowed access to the child actors.
	Common::FTDS::DynamicArray<Common::IActor*>* Common::ActorData::GetChildActors()
	{
		return Children;
	}

	/// @brief Returns the draw order used by this actor data.
	/// @return Current draw order.
	const int Common::ActorData::GetDrawOrder() const
	{
		return DrawOrder;
	}

	/// @brief Updates the name used by subsequent operations.
	/// @param name Replacement name.
	void Common::ActorData::SetName(Common::FTDS::String&& name)
	{
		Name.Assign(name);
	}

	/// @brief Updates the is active used by subsequent operations.
	/// @param isActive Replacement is active.
	void Common::ActorData::SetIsActive(bool isActive)
	{
		IsActive = isActive;
	}

	/// @brief Updates the actor group used by subsequent operations.
	/// @param group Replacement actor group.
	void Common::ActorData::SetActorGroup(Common::ActorGroup group)
	{
		ActorGroup = group;
	}

	/// @brief Updates the state used by subsequent operations.
	/// @param state Replacement state.
	void Common::ActorData::SetState(ActorState state)
	{
		State = state;
	}

	/// @brief Updates the parent used by subsequent operations.
	/// @param parent Replacement parent.
	void Common::ActorData::SetParent(Common::IActor* parent)
	{
		Parent = parent;
	}

	/// @brief Updates the transform used by subsequent operations.
	/// @param transform Replacement transform.
	void Common::ActorData::SetTransform(Core::Transform* transform)
	{
		Transform = transform;
	}

	/// @brief Updates the components used by subsequent operations.
	/// @param components Replacement components.
	void Common::ActorData::SetComponents(Common::FTDS::DynamicArray<IComponent*>* components)
	{
		Components = components;
	}

	/// @brief Updates the child actors used by subsequent operations.
	/// @param children Replacement child actors.
	void Common::ActorData::SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children)
	{
		Children = children;
	}

	/// @brief Updates the draw order used by subsequent operations.
	/// @param order Replacement draw order.
	void Common::ActorData::SetDrawOrder(int order)
	{
		DrawOrder = order;
	}

	/// @brief Compares the actor's stored name with the supplied name.
	/// @param name Name used to identify the requested object or interface.
	/// @return True when the compared values match; otherwise false.
	bool Common::ActorData::HasName(Common::FTDS::String&& name)
	{
		return Name.Equal(name.C_Str());
	}

	/// @brief Compares the actor's stored name with the supplied name.
	/// @param name Name used to identify the requested object or interface.
	/// @return True when the compared values match; otherwise false.
	bool Common::ActorData::HasName(const char* name)
	{
		return Name.Equal(name);
	}

	/// @brief Tests whether the actor state requests deferred destruction.
	/// @return True when the actor state requests deferred destruction; otherwise false.
	bool Common::ActorData::IsDead()
	{
		return State == ActorState::DEAD;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void Common::ActorData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);

		Common::FileIOHelper::SaveString(ofs, ChunkKey::NAME, Name);
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::ID::ID, ID);
		Transform->SaveProperties(ofs);
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::DRAW_ORDER, DrawOrder);
		Common::FileIOHelper::SaveString(ofs, ChunkKey::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(ActorGroup));
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::STATE, IsActive);

		if (Parent)
			Common::FileIOHelper::SaveInt(ofs, ChunkKey::PARENT, Parent->GetID());
		else
			Common::FileIOHelper::SaveString(ofs, ChunkKey::PARENT, Common::ChunkKey::NullVal::NULL_OBJECT);

		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHILD);

		if (0 < Children->GetSize())
		{
			for (size_t i = 0; i < Children->GetSize(); ++i)
				Common::FileIOHelper::SaveInt(ofs, std::to_string(i).c_str(), Children->At(i)->GetID());
		}

		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHILD);

		// Changing the call location of Transform is NOT recommended
		// Nested .chunk DataPack has unknown problem.
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_PROPERTIES);
	}

	/// @brief Serializes the actor's attached components to the .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	void Common::ActorData::SaveComponents(std::ofstream& ofs)
	{
		size_t count = Components->GetSize();
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::COMPONENTS);
		for (size_t i = 0; i < count; ++i)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, Components->At(i)->GetName());
			Components->At(i)->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, Components->At(i)->GetName());
		}
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::COMPONENTS);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void Common::ActorData::LoadProperties(std::ifstream& ifs)
	{
		// Load Actor state
		Common::FileIOHelper::LoadBool(ifs, IsActive);

		// Load Actor group
		Common::FTDS::String actorGroupStr;
		Common::FileIOHelper::LoadBasicString(ifs, actorGroupStr);
		ActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

		// Load Actor draw order
		Common::FileIOHelper::LoadInt(ifs, DrawOrder);

		// Load Transform
		Transform->LoadProperties(ifs);

		// Load int
		Common::FileIOHelper::LoadInt(ifs, ID);

		// Load Actor name
		Common::FileIOHelper::LoadBasicString(ifs, Name);
	}

	/// @brief Restores actor components through the available plugin factories.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void Common::ActorData::LoadComponents(std::ifstream& ifs)
	{
		std::pair<size_t, Common::FTDS::String>&& pack = Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::COMPONENTS);
		Components->Reserve(pack.first);
		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String> compPack = Common::FileIOHelper::BeginDataPackLoad(ifs);
			// ChunkLoader::GetInstance()->GetComponentLoadMap().At(compPack.second)->Value()(this, ifs);
		}
	}
} // namespace Common
