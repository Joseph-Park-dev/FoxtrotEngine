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

		/// @brief Releases the resources managed by this instance during destruction.
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

		/// @brief Constructs a component, inserts it by update order, and registers it with the plugin.
		/// @param plugin Plugin supplying component or lifecycle services.
		/// @return Component owned by the actor's component collection.
		/// @pre The actor's component collection and plugin must be initialized.
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

		/// @brief Finds the first attached component compatible with the requested type.
		/// @return Borrowed matching component, or nullptr when no compatible component exists.
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

		/// @brief Registers an actor in this actor's child collection.
		/// @param child Child actor to attach or detach.
		CORE_API void AddChild(Common::IActor* child);
		/// @brief Detaches an actor from this actor's child collection.
		/// @param child Child actor to attach or detach.
		CORE_API void RemoveChild(Common::IActor* child);
		/// @brief Removes the specified component from the actor's component collection.
		/// @param comp Component instance associated with the actor or plugin.
		CORE_API void RemoveComponent(Common::IComponent* comp);
		/// @brief Removes all components associated with the actor.
		CORE_API void RemoveAllComponents();

		// Deep copies transform from another Actor.
		/// @brief Copies spatial state from the source actor into this actor's transform.
		/// @param actor Actor participating in this operation.
		CORE_API void CopyTransformFrom(Common::IActor* actor);

		// Creates new IComponent with values from another Actor.
		/// @brief Clones the source actor's components for this actor.
		/// @param actor Actor participating in this operation.
		CORE_API void CopyComponentsFrom(Common::IActor* actor);

		// Deep copies all child Actors
		/// @brief Clones the source actor's child hierarchy.
		/// @param actor Actor participating in this operation.
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
		/// @brief Associates the source actor's children with this editor or runtime actor.
		/// @param actor Actor participating in this operation.
		CORE_API void RefChildObjectFrom(Common::IActor* actor);

		/// @brief Returns the actor group used by this actor data.
		/// @return Current actor group.
		CORE_API Common::ActorGroup							 GetActorGroup() const;
		/// @brief Returns the actor group ref used by this actor data.
		/// @return Borrowed access to the actor group ref.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API Common::ActorGroup&							 GetActorGroupRef();
		/// @brief Returns the actor group ptr used by this actor data.
		/// @return Borrowed access to the actor group ptr.
		CORE_API Common::ActorGroup*							 GetActorGroupPtr();
		/// @brief Returns the name used by this actor data.
		/// @return Current name.
		CORE_API Common::FTDS::String						 GetName();
		/// @brief Returns the name ref used by this actor data.
		/// @return Borrowed access to the name ref.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API Common::FTDS::String&						 GetNameRef();
		/// @brief Returns the id used by this actor data.
		/// @return Current id.
		CORE_API const int									 GetID() const;
		/// @brief Returns the is active used by this actor data.
		/// @return Current value of the is active flag.
		CORE_API const bool									 GetIsActive() const;
		/// @brief Returns the is active ref used by this actor data.
		/// @return Current value of the is active ref flag.
		CORE_API bool&										 GetIsActiveRef();
		/// @brief Returns the transform used by this actor data.
		/// @return Borrowed access to the transform.
		CORE_API Core::Transform*							 GetTransform() const;
		/// @brief Returns the parent used by this actor data.
		/// @return Borrowed access to the parent.
		CORE_API Common::IActor*								 GetParent() const;
		/// @brief Returns the components used by this actor data.
		/// @return Borrowed access to the components.
		CORE_API Common::FTDS::DynamicArray<IComponent*>*	 GetComponents();
		/// @brief Returns the child actors used by this actor data.
		/// @return Borrowed access to the child actors.
		CORE_API Common::FTDS::DynamicArray<Common::IActor*>* GetChildActors();
		/// @brief Returns the draw order used by this actor data.
		/// @return Current draw order.
		CORE_API const int									 GetDrawOrder() const;

		/// @brief Updates the name used by subsequent operations.
		/// @param name Replacement name.
		CORE_API void SetName(Common::FTDS::String&& name);
		/// @brief Updates the is active used by subsequent operations.
		/// @param isActive Replacement is active.
		CORE_API void SetIsActive(bool isActive);
		/// @brief Updates the actor group used by subsequent operations.
		/// @param group Replacement actor group.
		CORE_API void SetActorGroup(Common::ActorGroup group);
		/// @brief Updates the state used by subsequent operations.
		/// @param state Replacement state.
		CORE_API void SetState(Common::ActorState state);
		/// @brief Updates the parent used by subsequent operations.
		/// @param parent Replacement parent.
		CORE_API void SetParent(Common::IActor* parent);
		/// @brief Updates the transform used by subsequent operations.
		/// @param transform Replacement transform.
		CORE_API void SetTransform(Core::Transform* transform);
		/// @brief Updates the components used by subsequent operations.
		/// @param components Replacement components.
		CORE_API void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components);
		/// @brief Updates the child actors used by subsequent operations.
		/// @param children Replacement child actors.
		CORE_API void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children);
		/// @brief Updates the draw order used by subsequent operations.
		/// @param order Replacement draw order.
		CORE_API void SetDrawOrder(int order);

		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		CORE_API bool HasName(Common::FTDS::String&& name);
		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		CORE_API bool HasName(const char* name);
		/// @brief Tests whether the actor state requests deferred destruction.
		/// @return True when the actor state requests deferred destruction; otherwise false.
		CORE_API bool IsDead();

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		CORE_API void SaveProperties(std::ofstream& ofs);
		/// @brief Serializes the actor's attached components to the .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		CORE_API void SaveComponents(std::ofstream& ofs);

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		CORE_API void LoadProperties(std::ifstream& ifs);
		/// @brief Restores actor components through the available plugin factories.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		CORE_API void LoadComponents(std::ifstream& ifs);
	};

	namespace ChunkKey
	{
		constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	}
} // namespace Common
