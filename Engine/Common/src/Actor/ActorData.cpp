#include "Actor/ActorData.h"

#include "Actor/IActor.h"
#include "Actor/ActorGroup.h"
#include "FileSystem/FileIOHelper.h"
#include "Actor/Transform.h"

namespace Common
{
	void Common::ActorData::AddChild(IActor* child)
	{
		child->SetParent(Owner);
		Children->PushBack(child);
	}

	void Common::ActorData::RemoveChild(IActor* child)
	{
		int pos = Children->Find(child);
		if (pos == -1)
			return;

		Children->Erase(pos);

		// if (child->GetParent()->GetParent())
		//	child->SetParent(child->GetParent()->GetParent());
	}

	void Common::ActorData::RemoveComponent(IComponent* comp)
	{
		int pos = Components->Find(comp);
		if (pos == -1)
			return;

		delete comp;
		comp = nullptr;
		Components->Erase(pos);
	}

	void Common::ActorData::RemoveAllComponents()
	{
		for (auto comp = Components->Begin(); comp != Components->End(); ++comp)
			delete *comp;
		Components->Clear();
	}

	void ActorData::CopyTransformFrom(IActor* actor)
	{
		actor->GetTransform()->CloneTo(Transform);
	}

	void ActorData::CopyComponentsFrom(IActor* actor)
	{
		this->RemoveAllComponents();

		Common::FTDS::DynamicArray<IComponent*>* compsToCopy = actor->GetComponents();
		for (size_t i = 0; i < compsToCopy->GetSize(); ++i)
			compsToCopy->At(i)->CloneTo(Owner);
	}

	void ActorData::RefChildObjectFrom(IActor* actor)
	{
		if (actor->GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](IActor* child) {
			this->AddChild(child);
		});
	}

	Common::ActorGroup Common::ActorData::GetActorGroup() const
	{
		return ActorGroup;
	}

	Common::ActorGroup& Common::ActorData::GetActorGroupRef()
	{
		return ActorGroup;
	}

	Common::ActorGroup* Common::ActorData::GetActorGroupPtr()
	{
		return &ActorGroup;
	}

	Common::FTDS::String Common::ActorData::GetName()
	{
		return Name;
	}

	Common::FTDS::String& Common::ActorData::GetNameRef()
	{
		return Name;
	}

	const int Common::ActorData::GetID() const
	{
		return ID;
	}

	const bool Common::ActorData::GetIsActive() const
	{
		return IsActive;
	}

	bool& ActorData::GetIsActiveRef()
	{
		return IsActive;
	}

	Common::Transform* Common::ActorData::GetTransform() const
	{
		return Transform;
	}

	IActor* Common::ActorData::GetParent() const
	{
		return Parent;
	}

	Common::FTDS::DynamicArray<IComponent*>* Common::ActorData::GetComponents()
	{
		return Components;
	}

	Common::FTDS::DynamicArray<IActor*>* Common::ActorData::GetChildActors()
	{
		return Children;
	}

	const int Common::ActorData::GetDrawOrder() const
	{
		return DrawOrder;
	}

	void Common::ActorData::SetName(Common::FTDS::String&& name)
	{
		Name.Assign(name);
	}

	void Common::ActorData::SetIsActive(bool isActive)
	{
		IsActive = isActive;
	}

	void Common::ActorData::SetActorGroup(Common::ActorGroup group)
	{
		ActorGroup = group;
	}

	void Common::ActorData::SetState(ActorState state)
	{
		State = state;
	}

	void Common::ActorData::SetParent(IActor* parent)
	{
		Parent = parent;
	}

	void Common::ActorData::SetTransform(Common::Transform* transform)
	{
		Transform = transform;
	}

	void Common::ActorData::SetComponents(Common::FTDS::DynamicArray<IComponent*>* components)
	{
		Components = components;
	}

	void Common::ActorData::SetChildActors(Common::FTDS::DynamicArray<IActor*>* children)
	{
		Children = children;
	}

	void Common::ActorData::SetDrawOrder(int order)
	{
		DrawOrder = order;
	}

	bool Common::ActorData::HasName(Common::FTDS::String&& name)
	{
		return Name.Equal(name.C_Str());
	}

	bool Common::ActorData::HasName(const char* name)
	{
		return Name.Equal(name);
	}

	bool Common::ActorData::IsDead()
	{
		return State == ActorState::DEAD;
	}

	void ActorData::SaveProperties(std::ofstream& ofs)
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

	void ActorData::SaveComponents(std::ofstream& ofs)
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

	void ActorData::LoadProperties(std::ifstream& ifs)
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

	void ActorData::LoadComponents(std::ifstream& ifs)
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