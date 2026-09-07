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
		/// <summary>
		/// Creates an empty Actor with zero-initialized values, but doesn't add it to the Scene.
		/// </summary>
		CORE_API Actor(int id);

		/// <summary>
		/// Copy constructors that adds a deep-copied Actor to the scene.
		/// </summary>
		/// <param name="actor : "> Actor being copied.</param>
		CORE_API Actor(Common::IActor* actor, int id);

		/// <summary>
		/// Copy constructors controlling if the object is deep copied.
		/// </summary>
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		CORE_API Actor(Common::IActor* actor, int id, bool deepCpyChild);

		/// <summary>
		/// Copies the origin from the FTPremade into this object, adding it to the scene
		/// </summary>
		/// <param name="premade : ">Premade to copy origin from.</param>
		/// <param name="scene : ">A scene object to add this object to.</param>
		CORE_API Actor(Core::FTPremade* premade, int id);

		CORE_API virtual ~Actor();

	public:
		CORE_API virtual void AddChild(Common::IActor* actor) override;
		CORE_API virtual void RemoveChild(Common::IActor* actor) override;
		CORE_API virtual void RemoveComponent(Common::IComponent* component) override;
		CORE_API virtual void RemoveAllComponents() override;

	public:
		// Deep copies transform from another Actor.
		CORE_API virtual void CopyTransformFrom(Common::IActor* actor) override;

		// Creates new IComponent with values from another Actor.
		CORE_API virtual void CopyComponentsFrom(Common::IActor* actor) override;

		// Deep copies all child Actors
		CORE_API virtual void CopyChildObjectFrom(Common::IActor* actor) override;

		// Shallow copies all child Actors.
		CORE_API virtual void RefChildObjectFrom(Common::IActor* actor) override;

	public:
		// Getters/Setters
		CORE_API virtual Common::ActorData*								 GetData() override;
		CORE_API virtual Common::ActorGroup								 GetActorGroup() const override;
		CORE_API virtual Common::ActorGroup&								 GetActorGroupRef() override;
		CORE_API virtual Common::ActorGroup*								 GetActorGroupPtr() override;
		CORE_API virtual Common::FTDS::String							 GetName() override;
		CORE_API virtual Common::FTDS::String&							 GetNameRef() override;
		CORE_API virtual const int										 GetID() const override;
		CORE_API virtual const bool&										 GetIsActive() const override;
		CORE_API virtual bool&											 GetIsActiveRef() override;
		CORE_API virtual Transform*										 GetTransform() const override;
		CORE_API virtual Actor*											 GetParent() const override;
		CORE_API virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents() override;
		CORE_API virtual Common::FTDS::DynamicArray<Common::IActor*>*	 GetChildActors() override;
		CORE_API virtual const int&										 GetDrawOrder() const override;

		CORE_API virtual void SetName(Common::FTDS::String&& name) override;
		CORE_API virtual void SetIsActive(bool isActive) override;
		CORE_API virtual void SetActorGroup(Common::ActorGroup group) override;
		CORE_API virtual void SetState(Common::ActorState state) override;
		CORE_API virtual void SetParent(Common::IActor* parent) override;
		CORE_API virtual void SetTransform(Core::Transform* transform) override;
		CORE_API virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) override;
		CORE_API virtual void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children) override;
		CORE_API virtual void SetDrawOrder(int order) override;

		CORE_API virtual bool HasName(Common::FTDS::String&& name) override;
		CORE_API virtual bool HasName(const char* name) override;
		CORE_API virtual bool IsDead() override;

	private:
		Common::ActorData* mData;

	public:
		CORE_API virtual void SaveProperties(std::ofstream& ofs) override;
		CORE_API virtual void SaveComponents(std::ofstream& ofs) override;

		CORE_API virtual void LoadProperties(std::ifstream& ifs) override;
		CORE_API virtual void LoadComponents(std::ifstream& ifs) override;
	};
} // namespace Core