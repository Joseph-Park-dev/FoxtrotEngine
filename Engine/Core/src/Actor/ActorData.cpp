#include "Actor/ActorData.h"

#include "Actor/IActor.h"
#include "Actor/ActorGroup.h"
#include "FileSystem/FileIOHelper.h"
#include "Actor/Transform.h"

namespace Core
{
	void Core::ActorData::AddChild(IActor* child)
	{
		child->SetParent(Owner);
		Children->PushBack(child);
	}

	void Core::ActorData::RemoveChild(IActor* child)
	{
		int pos = Children->Find(child);
		if (pos == -1)
			return;

		Children->Erase(pos);

		//if (child->GetParent()->GetParent())
		//	child->SetParent(child->GetParent()->GetParent());
	}

	void Core::ActorData::RemoveComponent(IComponent* comp)
	{
		int pos = Components->Find(comp);
		if (pos == -1)
			return;

		delete comp;
		comp = nullptr;
		Components->Erase(pos);
	}

	void Core::ActorData::RemoveAllComponents()
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

	Core::ActorGroup Core::ActorData::GetActorGroup() const
	{
		return ActorGroup;
	}

	Core::ActorGroup& Core::ActorData::GetActorGroupRef()
	{
		return ActorGroup;
	}

	Core::ActorGroup* Core::ActorData::GetActorGroupPtr()
	{
		return &ActorGroup;
	}

	Common::FTDS::String Core::ActorData::GetName()
	{
		return Name;
	}

	Common::FTDS::String& Core::ActorData::GetNameRef()
	{
		return Name;
	}

	const int Core::ActorData::GetID() const
	{
		return ID;
	}

	const bool Core::ActorData::GetIsActive() const
	{
		return IsActive;
	}

	bool& ActorData::GetIsActiveRef()
	{
		return IsActive;
	}

	Core::Transform* Core::ActorData::GetTransform() const
	{
		return Transform;
	}

	IActor* Core::ActorData::GetParent() const
	{
		return Parent;
	}

	Common::FTDS::DynamicArray<IComponent*>* Core::ActorData::GetComponents()
	{
		return Components;
	}

	Common::FTDS::DynamicArray<IActor*>* Core::ActorData::GetChildActors()
	{
		return Children;
	}

	const int Core::ActorData::GetDrawOrder() const
	{
		return DrawOrder;
	}

	void Core::ActorData::SetName(Common::FTDS::String&& name)
	{
		Name.Assign(name);
	}

	void Core::ActorData::SetIsActive(bool isActive)
	{
		IsActive = isActive;
	}

	void Core::ActorData::SetActorGroup(Core::ActorGroup group)
	{
		ActorGroup = group;
	}

	void Core::ActorData::SetState(ActorState state)
	{
		State = state;
	}

	void Core::ActorData::SetParent(IActor* parent)
	{
		Parent = parent;
	}

	void Core::ActorData::SetTransform(Core::Transform* transform)
	{
		Transform = transform;
	}

	void Core::ActorData::SetComponents(Common::FTDS::DynamicArray<IComponent*>* components)
	{
		Components = components;
	}

	void Core::ActorData::SetChildActors(Common::FTDS::DynamicArray<IActor*>* children)
	{
		Children = children;
	}

	void Core::ActorData::SetDrawOrder(int order)
	{
		DrawOrder = order;
	}

	bool Core::ActorData::HasName(Common::FTDS::String&& name)
	{
		return Name.Equal(name.C_Str());
	}

	bool Core::ActorData::HasName(const char* name)
	{
		return Name.Equal(name);
	}

	bool Core::ActorData::IsDead()
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
} // namespace Core