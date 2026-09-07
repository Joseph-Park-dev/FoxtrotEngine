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
		virtual void AddChild(Common::IActor* actor)				= 0;
		virtual void RemoveChild(Common::IActor* actor)				= 0;
		virtual void RemoveComponent(Common::IComponent* component) = 0;
		virtual void RemoveAllComponents()							= 0;

	public:
		// Deep copies transform from another Common::IActor.
		virtual void CopyTransformFrom(Common::IActor* actor) = 0;

		// Creates new IComponent with values from another Common::IActor.
		virtual void CopyComponentsFrom(Common::IActor* actor) = 0;

		// Deep copies all child Actors
		virtual void CopyChildObjectFrom(Common::IActor* actor) = 0;

		// Shallow copies all child Actors.
		virtual void RefChildObjectFrom(Common::IActor* actor) = 0;

	public:
		// Getters/Setters
		virtual Common::ActorData*								 GetData()			   = 0;
		virtual Common::ActorGroup								 GetActorGroup() const = 0;
		virtual Common::ActorGroup&								 GetActorGroupRef()	   = 0;
		virtual Common::ActorGroup*								 GetActorGroupPtr()	   = 0;
		virtual Common::FTDS::String							 GetName()			   = 0;
		virtual Common::FTDS::String&							 GetNameRef()		   = 0;
		virtual const int										 GetID() const		   = 0;
		virtual const bool&										 GetIsActive() const   = 0;
		virtual bool&											 GetIsActiveRef()	   = 0;
		virtual Core::Transform*								 GetTransform() const  = 0;
		virtual Common::IActor*									 GetParent() const	   = 0;
		virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents()	   = 0;
		virtual Common::FTDS::DynamicArray<Common::IActor*>*	 GetChildActors()	   = 0;
		virtual const int&										 GetDrawOrder() const  = 0;

		virtual void SetName(Common::FTDS::String&& name)										= 0;
		virtual void SetIsActive(bool isActive)													= 0;
		virtual void SetActorGroup(Common::ActorGroup group)									= 0;
		virtual void SetState(Common::ActorState state)											= 0;
		virtual void SetParent(Common::IActor* parent)											= 0;
		virtual void SetTransform(Core::Transform* transform)									= 0;
		virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) = 0;
		virtual void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children)		= 0;
		virtual void SetDrawOrder(int order)													= 0;

		virtual bool HasName(Common::FTDS::String&& name) = 0;
		virtual bool HasName(const char* name)			  = 0;
		virtual bool IsDead()							  = 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void SaveComponents(std::ofstream& ofs) = 0;

		virtual void LoadProperties(std::ifstream& ifs) = 0;
		virtual void LoadComponents(std::ifstream& ifs) = 0;

	public:
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