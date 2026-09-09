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
#include <iosfwd>

#include "FTDS/Static/FTString.h"

namespace Core
{
	class Transform;
}

namespace Common
{
	class IComponent;
	struct ActorData;
	enum class ActorGroup;
	enum class ActorState;

	class IActor
	{
	public:
		/// @brief Registers an actor in this actor's child collection.
		/// @param actor Actor participating in this operation.
		virtual void AddChild(Common::IActor* actor)				= 0;
		/// @brief Detaches an actor from this actor's child collection.
		/// @param actor Actor participating in this operation.
		virtual void RemoveChild(Common::IActor* actor)				= 0;
		/// @brief Removes the specified component from the actor's component collection.
		/// @param component Component instance associated with the actor or plugin.
		virtual void RemoveComponent(Common::IComponent* component) = 0;
		/// @brief Removes all components associated with the actor.
		virtual void RemoveAllComponents()							= 0;

	public:
		// Deep copies transform from another Common::IActor.
		/// @brief Copies spatial state from the source actor into this actor's transform.
		/// @param actor Actor participating in this operation.
		virtual void CopyTransformFrom(Common::IActor* actor) = 0;

		// Creates new IComponent with values from another Common::IActor.
		/// @brief Clones the source actor's components for this actor.
		/// @param actor Actor participating in this operation.
		virtual void CopyComponentsFrom(Common::IActor* actor) = 0;

		// Deep copies all child Actors
		/// @brief Clones the source actor's child hierarchy.
		/// @param actor Actor participating in this operation.
		virtual void CopyChildObjectFrom(Common::IActor* actor) = 0;

		// Shallow copies all child Actors.
		/// @brief Associates the source actor's children with this editor or runtime actor.
		/// @param actor Actor participating in this operation.
		virtual void RefChildObjectFrom(Common::IActor* actor) = 0;

	public:
		// Getters/Setters
		/// @brief Returns the data used by this iactor.
		/// @return Borrowed access to the data.
		virtual Common::ActorData*								 GetData()			   = 0;
		/// @brief Returns the actor group used by this iactor.
		/// @return Current actor group.
		virtual Common::ActorGroup								 GetActorGroup() const = 0;
		/// @brief Returns the actor group ref used by this iactor.
		/// @return Borrowed access to the actor group ref.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Common::ActorGroup&								 GetActorGroupRef()	   = 0;
		/// @brief Returns the actor group ptr used by this iactor.
		/// @return Borrowed access to the actor group ptr.
		virtual Common::ActorGroup*								 GetActorGroupPtr()	   = 0;
		/// @brief Returns the name used by this iactor.
		/// @return Current name.
		virtual Common::FTDS::String							 GetName()			   = 0;
		/// @brief Returns the name ref used by this iactor.
		/// @return Borrowed access to the name ref.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Common::FTDS::String&							 GetNameRef()		   = 0;
		/// @brief Returns the id used by this iactor.
		/// @return Current id.
		virtual const int										 GetID() const		   = 0;
		/// @brief Returns the is active used by this iactor.
		/// @return Current value of the is active flag.
		virtual const bool&										 GetIsActive() const   = 0;
		/// @brief Returns the is active ref used by this iactor.
		/// @return Current value of the is active ref flag.
		virtual bool&											 GetIsActiveRef()	   = 0;
		/// @brief Returns the transform used by this iactor.
		/// @return Borrowed access to the transform.
		virtual Core::Transform*								 GetTransform() const  = 0;
		/// @brief Returns the parent used by this iactor.
		/// @return Borrowed access to the parent.
		virtual Common::IActor*									 GetParent() const	   = 0;
		/// @brief Returns the components used by this iactor.
		/// @return Borrowed access to the components.
		virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents()	   = 0;
		/// @brief Returns the child actors used by this iactor.
		/// @return Borrowed access to the child actors.
		virtual Common::FTDS::DynamicArray<Common::IActor*>*	 GetChildActors()	   = 0;
		/// @brief Returns the draw order used by this iactor.
		/// @return Borrowed access to the draw order.
		virtual const int&										 GetDrawOrder() const  = 0;

		/// @brief Updates the name used by subsequent operations.
		/// @param name Replacement name.
		virtual void SetName(Common::FTDS::String&& name)										= 0;
		/// @brief Updates the is active used by subsequent operations.
		/// @param isActive Replacement is active.
		virtual void SetIsActive(bool isActive)													= 0;
		/// @brief Updates the actor group used by subsequent operations.
		/// @param group Replacement actor group.
		virtual void SetActorGroup(Common::ActorGroup group)									= 0;
		/// @brief Updates the state used by subsequent operations.
		/// @param state Replacement state.
		virtual void SetState(Common::ActorState state)											= 0;
		/// @brief Updates the parent used by subsequent operations.
		/// @param parent Replacement parent.
		virtual void SetParent(Common::IActor* parent)											= 0;
		/// @brief Updates the transform used by subsequent operations.
		/// @param transform Replacement transform.
		virtual void SetTransform(Core::Transform* transform)									= 0;
		/// @brief Updates the components used by subsequent operations.
		/// @param components Replacement components.
		virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) = 0;
		/// @brief Updates the child actors used by subsequent operations.
		/// @param children Replacement child actors.
		virtual void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children)		= 0;
		/// @brief Updates the draw order used by subsequent operations.
		/// @param order Replacement draw order.
		virtual void SetDrawOrder(int order)													= 0;

		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		virtual bool HasName(Common::FTDS::String&& name) = 0;
		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		virtual bool HasName(const char* name)			  = 0;
		/// @brief Tests whether the actor state requests deferred destruction.
		/// @return True when the actor state requests deferred destruction; otherwise false.
		virtual bool IsDead()							  = 0;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		/// @brief Serializes the actor's attached components to the .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		virtual void SaveComponents(std::ofstream& ofs) = 0;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;
		/// @brief Restores actor components through the available plugin factories.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		virtual void LoadComponents(std::ifstream& ifs) = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IActor() = default;
	};

	namespace ChunkKey
	{
		constexpr const char* NAME		 = "Name";
		constexpr const char* DRAW_ORDER = "Draw Order";
		constexpr const char* STATE		 = "State";
		constexpr const char* PARENT	 = "Parent";
		constexpr const char* CHILD		 = "Child List";

		namespace ID
		{
			// ID for invalid, or temporary object
			constexpr int INVALID = -1;

			// ID for cloned object (instantiated object)
			constexpr int CLONE = 0;

			// ID for chunk title.
			constexpr const char* ID = "ID";
		} // namespace ID
	} // namespace ChunkKey
} // namespace Common
