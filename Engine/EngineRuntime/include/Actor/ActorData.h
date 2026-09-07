#pragma once
#include "Plugin/CoreExports.h"
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

		CORE_API ~ActorData()
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

		CORE_API void AddChild(Common::IActor* child);
		CORE_API void RemoveChild(Common::IActor* child);
		CORE_API void RemoveComponent(Common::IComponent* comp);
		CORE_API void RemoveAllComponents();

		// Deep copies transform from another Actor.
		CORE_API void CopyTransformFrom(Common::IActor* actor);

		// Creates new IComponent with values from another Actor.
		CORE_API void CopyComponentsFrom(Common::IActor* actor);

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
		CORE_API void RefChildObjectFrom(Common::IActor* actor);

		CORE_API Common::ActorGroup							 GetActorGroup() const;
		CORE_API Common::ActorGroup&							 GetActorGroupRef();
		CORE_API Common::ActorGroup*							 GetActorGroupPtr();
		CORE_API Common::FTDS::String						 GetName();
		CORE_API Common::FTDS::String&						 GetNameRef();
		CORE_API const int									 GetID() const;
		CORE_API const bool									 GetIsActive() const;
		CORE_API bool&										 GetIsActiveRef();
		CORE_API Core::Transform*							 GetTransform() const;
		CORE_API Common::IActor*								 GetParent() const;
		CORE_API Common::FTDS::DynamicArray<IComponent*>*	 GetComponents();
		CORE_API Common::FTDS::DynamicArray<Common::IActor*>* GetChildActors();
		CORE_API const int									 GetDrawOrder() const;

		CORE_API void SetName(Common::FTDS::String&& name);
		CORE_API void SetIsActive(bool isActive);
		CORE_API void SetActorGroup(Common::ActorGroup group);
		CORE_API void SetState(Common::ActorState state);
		CORE_API void SetParent(Common::IActor* parent);
		CORE_API void SetTransform(Core::Transform* transform);
		CORE_API void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components);
		CORE_API void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children);
		CORE_API void SetDrawOrder(int order);

		CORE_API bool HasName(Common::FTDS::String&& name);
		CORE_API bool HasName(const char* name);
		CORE_API bool IsDead();

		CORE_API void SaveProperties(std::ofstream& ofs);
		CORE_API void SaveComponents(std::ofstream& ofs);

		CORE_API void LoadProperties(std::ifstream& ifs);
		CORE_API void LoadComponents(std::ifstream& ifs);
	};

	namespace ChunkKey
	{
		constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	}
} // namespace Common
