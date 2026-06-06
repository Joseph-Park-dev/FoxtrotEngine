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
#include "Actor/IActor.h"

#include <fstream>

#include "FTDS/Static/FTString.h"

namespace Core
{
	class FTPremade;
	struct ActorData;

	class Actor :
		public Common::IActor
	{
	public:
		/// <summary>
		/// Creates an empty Actor with zero-initialized values, but doesn't add it to the Scene.
		/// </summary>
		Actor(int id);

		/// <summary>
		/// Copy constructors that adds a deep-copied Actor to the scene.
		/// </summary>
		/// <param name="actor : "> Actor being copied.</param>
		Actor(Common::IActor* actor, int id);

		/// <summary>
		/// Copy constructors controlling if the object is deep copied.
		/// </summary>
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		Actor(Common::IActor* actor, int id, bool deepCpyChild);

		/// <summary>
		/// Copies the origin from the FTPremade into this object, adding it to the scene
		/// </summary>
		/// <param name="premade : ">Premade to copy origin from.</param>
		/// <param name="scene : ">A scene object to add this object to.</param>
		Actor(Core::FTPremade* premade, int id);

		virtual ~Actor();

	public:
		virtual void AddChild(Common::IActor* actor) override;
		virtual void RemoveChild(Common::IActor* actor) override;
		virtual void RemoveComponent(Common::IComponent* component) override;
		virtual void RemoveAllComponents() override;

	public:
		// Deep copies transform from another Actor.
		virtual void CopyTransformFrom(Common::IActor* actor) override;

		// Creates new IComponent with values from another Actor.
		virtual void CopyComponentsFrom(Common::IActor* actor) override;

		// Deep copies all child Actors
		virtual void CopyChildObjectFrom(Common::IActor* actor) override;

		// Shallow copies all child Actors.
		virtual void RefChildObjectFrom(Common::IActor* actor) override;

	public:
		// Getters/Setters
		virtual Common::ActorData*								 GetData() override;
		virtual Common::ActorGroup								 GetActorGroup() const override;
		virtual Common::ActorGroup&								 GetActorGroupRef() override;
		virtual Common::ActorGroup*								 GetActorGroupPtr() override;
		virtual Common::FTDS::String							 GetName() override;
		virtual Common::FTDS::String&							 GetNameRef() override;
		virtual const int										 GetID() const override;
		virtual const bool&										 GetIsActive() const override;
		virtual bool&											 GetIsActiveRef() override;
		virtual Transform*										 GetTransform() const override;
		virtual Actor*											 GetParent() const override;
		virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents() override;
		virtual Common::FTDS::DynamicArray<Common::IActor*>*	 GetChildActors() override;
		virtual const int&										 GetDrawOrder() const override;

		virtual void SetName(Common::FTDS::String&& name) override;
		virtual void SetIsActive(bool isActive) override;
		virtual void SetActorGroup(Common::ActorGroup group) override;
		virtual void SetState(Common::ActorState state) override;
		virtual void SetParent(Common::IActor* parent) override;
		virtual void SetTransform(Core::Transform* transform) override;
		virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) override;
		virtual void SetChildActors(Common::FTDS::DynamicArray<Common::IActor*>* children) override;
		virtual void SetDrawOrder(int order) override;

		virtual bool HasName(Common::FTDS::String&& name) override;
		virtual bool HasName(const char* name) override;
		virtual bool IsDead() override;

	private:
		Common::ActorData* mData;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void SaveComponents(std::ofstream& ofs) override;

		virtual void LoadProperties(std::ifstream& ifs) override;
		virtual void LoadComponents(std::ifstream& ifs) override;
	};
} // namespace Core