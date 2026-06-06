#pragma once
#include "FTDS/Static/FTString.h"
#include "FileSystem/NullKeys.h"
#include "Actor/ActorGroup.h"
#include "Component/IComponent.h"
#include "Plugin/IPlugin.h"
#include "Actor/Transform.h"
#include "Actor/IActor.h"
#include "FileSystem/ChunkLoader.h"

namespace Common
{
	enum class ActorState
	{
		ALIVE,
		DEAD
	};

	namespace DefaultVal
	{
		constexpr int DRAW_ORDER = 0;
	}

	class IActor;

	struct ActorData
	{
		Common::IActor*								 Owner		= nullptr;
		Common::FTDS::String						 Name		= {};
		int											 ID			= Common::ChunkKey::NullVal::INVALID_IDX;
		Common::ActorGroup							 ActorGroup = Common::ActorGroup::DEFAULT;
		Common::ActorState							 State		= ActorState::ALIVE;
		bool										 IsActive	= true;
		Core::Transform*							 Transform	= nullptr;
		Common::FTDS::DynamicArray<IComponent*>*	 Components = nullptr;
		Common::IActor*								 Parent		= nullptr;
		Common::FTDS::DynamicArray<Common::IActor*>* Children	= nullptr;
		int											 DrawOrder	= Common::DefaultVal::DRAW_ORDER;

		~ActorData()
		{
			if (Transform)
			{
				delete Transform;
				Transform = nullptr;
			}

			for (size_t i = 0; i < Components->Size(); ++i)
			{
				delete Components->At(i);
				Components->At(i) = nullptr;
			}
			delete Components;

			for (size_t i = 0; i < Children->Size(); ++i)
			{
				delete Children->At(i);
				Children->At(i) = nullptr;
			}
			delete Children;

			Parent = nullptr;
		}

		template <typename COMP>
		COMP* AddComponent(IPlugin* plugin)
		{
			COMP*  comp		   = new COMP(Owner);
			int	   updateOrder = comp->GetUpdateOrder();
			auto   iter		   = Components->Begin();
			size_t iterPos	   = 0;
			for (; iter != Components->End(); ++iter)
			{
				if (!(*iter))
					break;

				if (updateOrder < (*iter)->GetUpdateOrder())
					break;
				++iterPos;
			}
			Components->Insert(iterPos, comp);
			plugin->RegisterComponent(comp);
			return comp;
		}

		template <class COMP>
		COMP* GetComponent()
		{
			for (auto iter = Components->Begin(); iter != Components->End(); ++iter)
			{
				COMP* comp = dynamic_cast<COMP*>(*iter);
				if (comp)
					return comp;
			}
			return nullptr;
		};

		void AddChild(Common::IActor* child);
		void RemoveChild(Common::IActor* child);
		void RemoveComponent(Common::IComponent* comp);
		void RemoveAllComponents();

		// Deep copies transform from another Actor.
		void CopyTransformFrom(Common::IActor* actor);

		// Creates new IComponent with values from another Actor.
		void CopyComponentsFrom(Common::IActor* actor);

		// Deep copies all child Actors
		template <typename ACTOR_INSTANCE>
		void CopyChildObjectFrom(Common::IActor* actor)
		{
			if (GetChildActors()->GetSize() < 1)
				return;

			actor->GetChildActors()->IterateArray([&](Common::IActor* child) {
				if (child)
				{
					ACTOR_INSTANCE* childAc = reinterpret_cast<ACTOR_INSTANCE*>(child);
					Core::ChunkLoader::GetInstance()->AddMaxActorID();
					int maxID = Core::ChunkLoader::GetInstance()->GetMaxActorID();
					this->AddChild(DBG_NEW ACTOR_INSTANCE(childAc, maxID));
				}
			});
		}

		// Shallow copies all child Actors.
		void RefChildObjectFrom(Common::IActor* actor);

		Common::ActorGroup							 GetActorGroup() const;
		Common::ActorGroup&							 GetActorGroupRef();
		Common::ActorGroup*							 GetActorGroupPtr();
		Common::FTDS::String						 GetName();
		Common::FTDS::String&						 GetNameRef();
		const int									 GetID() const;
		const bool									 GetIsActive() const;
		bool&										 GetIsActiveRef();
		Core::Transform*							 GetTransform() const;
		Common::IActor*								 GetParent() const;
		Common::FTDS::DynamicArray<IComponent*>*	 GetComponents();
		Common::FTDS::DynamicArray<Common::IActor*>* GetChildActors();
		const int									 GetDrawOrder() const;

		void SetName(Common::FTDS::String&& name);
		void SetIsActive(bool isActive);
		void SetActorGroup(Common::ActorGroup group);
		void SetState(Common::ActorState state);
		void SetParent(Common::IActor* parent);
		void SetTransform(Core::Transform* transform);
		void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components);
		void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children);
		void SetDrawOrder(int order);

		bool HasName(Common::FTDS::String&& name);
		bool HasName(const char* name);
		bool IsDead();

		void SaveProperties(std::ofstream& ofs);
		void SaveComponents(std::ofstream& ofs);

		void LoadProperties(std::ifstream& ifs);
		void LoadComponents(std::ifstream& ifs);
	};

	namespace ChunkKey
	{
		constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	}
} // namespace Common