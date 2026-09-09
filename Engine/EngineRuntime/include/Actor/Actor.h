// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// An object that can be placed into a Scene.
/// An Actor is consist of Transformation, Components, Behaviours, etc.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "Actor/IActor.h"

#include <fstream>

#include "FTDS/Static/FTString.h"

namespace Core
{
	class FTPremade;

	class Actor :
		public Common::IActor
	{
	public:
		/// @brief Initializes an actor's identity, transform, and component/child collections.
		/// Creates an empty Actor with zero-initialized values, but doesn't add it to the Scene.
		/// @param id Actor or entry identifier.
		CORE_API Actor(int id);

		/// @brief Initializes an actor's identity, transform, and component/child collections.
		/// Copy constructors that adds a deep-copied Actor to the scene.
		/// <param name="actor : "> Actor being copied.</param>
		/// @param actor Actor participating in this operation.
		/// @param id Actor or entry identifier.
		CORE_API Actor(Common::IActor* actor, int id);

		/// @brief Initializes an actor's identity, transform, and component/child collections.
		/// Copy constructors controlling if the object is deep copied.
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		/// @param actor Actor participating in this operation.
		/// @param id Actor or entry identifier.
		/// @param deepCpyChild Whether child actors are cloned instead of sharing references.
		CORE_API Actor(Common::IActor* actor, int id, bool deepCpyChild);

		/// @brief Initializes an actor's identity, transform, and component/child collections.
		/// Copies the origin from the FTPremade into this object, adding it to the scene
		/// <param name="premade : ">Premade to copy origin from.</param>
		/// <param name="scene : ">A scene object to add this object to.</param>
		/// @param premade Premade resource used as an actor template.
		/// @param id Actor or entry identifier.
		CORE_API Actor(Core::FTPremade* premade, int id);

		/// @brief Releases the resources managed by this instance during destruction.
		CORE_API virtual ~Actor();

	public:
		/// @brief Registers an actor in this actor's child collection.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void AddChild(Common::IActor* actor) override;
		/// @brief Detaches an actor from this actor's child collection.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void RemoveChild(Common::IActor* actor) override;
		/// @brief Removes the specified component from the actor's component collection.
		/// @param component Component instance associated with the actor or plugin.
		CORE_API virtual void RemoveComponent(Common::IComponent* component) override;
		/// @brief Removes all components associated with the actor.
		CORE_API virtual void RemoveAllComponents() override;

	public:
		// Deep copies transform from another Actor.
		/// @brief Copies spatial state from the source actor into this actor's transform.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void CopyTransformFrom(Common::IActor* actor) override;

		// Creates new IComponent with values from another Actor.
		/// @brief Clones the source actor's components for this actor.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void CopyComponentsFrom(Common::IActor* actor) override;

		// Deep copies all child Actors
		/// @brief Clones the source actor's child hierarchy.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void CopyChildObjectFrom(Common::IActor* actor) override;

		// Shallow copies all child Actors.
		/// @brief Associates the source actor's children with this editor or runtime actor.
		/// @param actor Actor participating in this operation.
		CORE_API virtual void RefChildObjectFrom(Common::IActor* actor) override;

	public:
		// Getters/Setters
		/// @brief Returns the data used by this actor.
		/// @return Borrowed access to the data.
		CORE_API virtual Common::ActorData*								 GetData() override;
		/// @brief Returns the actor group used by this actor.
		/// @return Current actor group.
		CORE_API virtual Common::ActorGroup								 GetActorGroup() const override;
		/// @brief Returns the actor group ref used by this actor.
		/// @return Borrowed access to the actor group ref.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API virtual Common::ActorGroup&								 GetActorGroupRef() override;
		/// @brief Returns the actor group ptr used by this actor.
		/// @return Borrowed access to the actor group ptr.
		CORE_API virtual Common::ActorGroup*								 GetActorGroupPtr() override;
		/// @brief Returns the name used by this actor.
		/// @return Current name.
		CORE_API virtual Common::FTDS::String							 GetName() override;
		/// @brief Returns the name ref used by this actor.
		/// @return Borrowed access to the name ref.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API virtual Common::FTDS::String&							 GetNameRef() override;
		/// @brief Returns the id used by this actor.
		/// @return Current id.
		CORE_API virtual const int										 GetID() const override;
		/// @brief Returns the is active used by this actor.
		/// @return Current value of the is active flag.
		CORE_API virtual const bool&										 GetIsActive() const override;
		/// @brief Returns the is active ref used by this actor.
		/// @return Current value of the is active ref flag.
		CORE_API virtual bool&											 GetIsActiveRef() override;
		/// @brief Returns the transform used by this actor.
		/// @return Borrowed access to the transform.
		CORE_API virtual Transform*										 GetTransform() const override;
		/// @brief Returns the parent used by this actor.
		/// @return Borrowed access to the parent.
		CORE_API virtual Actor*											 GetParent() const override;
		/// @brief Returns the components used by this actor.
		/// @return Borrowed access to the components.
		CORE_API virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents() override;
		/// @brief Returns the child actors used by this actor.
		/// @return Borrowed access to the child actors.
		CORE_API virtual Common::FTDS::DynamicArray<Common::IActor*>*	 GetChildActors() override;
		/// @brief Returns the draw order used by this actor.
		/// @return Borrowed access to the draw order.
		CORE_API virtual const int&										 GetDrawOrder() const override;

		/// @brief Updates the name used by subsequent operations.
		/// @param name Replacement name.
		CORE_API virtual void SetName(Common::FTDS::String&& name) override;
		/// @brief Updates the is active used by subsequent operations.
		/// @param isActive Replacement is active.
		CORE_API virtual void SetIsActive(bool isActive) override;
		/// @brief Updates the actor group used by subsequent operations.
		/// @param group Replacement actor group.
		CORE_API virtual void SetActorGroup(Common::ActorGroup group) override;
		/// @brief Updates the state used by subsequent operations.
		/// @param state Replacement state.
		CORE_API virtual void SetState(Common::ActorState state) override;
		/// @brief Updates the parent used by subsequent operations.
		/// @param parent Replacement parent.
		CORE_API virtual void SetParent(Common::IActor* parent) override;
		/// @brief Updates the transform used by subsequent operations.
		/// @param transform Replacement transform.
		CORE_API virtual void SetTransform(Core::Transform* transform) override;
		/// @brief Updates the components used by subsequent operations.
		/// @param components Replacement components.
		CORE_API virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) override;
		/// @brief Updates the child actors used by subsequent operations.
		/// @param children Replacement child actors.
		CORE_API virtual void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children) override;
		/// @brief Updates the draw order used by subsequent operations.
		/// @param order Replacement draw order.
		CORE_API virtual void SetDrawOrder(int order) override;

		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		CORE_API virtual bool HasName(Common::FTDS::String&& name) override;
		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		CORE_API virtual bool HasName(const char* name) override;
		/// @brief Tests whether the actor state requests deferred destruction.
		/// @return True when the actor state requests deferred destruction; otherwise false.
		CORE_API virtual bool IsDead() override;

	private:
		Common::ActorData* mData;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		CORE_API virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Serializes the actor's attached components to the .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		CORE_API virtual void SaveComponents(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		CORE_API virtual void LoadProperties(std::ifstream& ifs) override;
		/// @brief Restores actor components through the available plugin factories.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		CORE_API virtual void LoadComponents(std::ifstream& ifs) override;
	};
} // namespace Core
