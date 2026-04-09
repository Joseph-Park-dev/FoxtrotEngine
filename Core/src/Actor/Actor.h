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
#include <fstream>

#include "FTDS/Static/FTString.h"

namespace Core
{
	class Transform;
	class FTInputDevice;
	class Scene;
	class Component;
	class FoxtrotRenderer;
	class FTPremade;
	enum class ACTOR_TAG;
	enum class ActorGroup;
	namespace FTDS
	{
		template <typename TYPE>
		class DynamicArray;
		class String;
	} // namespace FTDS

#ifdef FOXTROT_EDITOR
	class EditorElement;
	class EditorScene;
#endif // FOXTROT_EDITOR

	enum class ActorState
	{
		ALIVE,
		DEAD
	};

	class Actor
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
		Actor(Actor* actor, int id);

		/// <summary>
		/// Copy constructors controlling if the object is deep copied.
		/// </summary>
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		Actor(Actor* actor, int id, bool deepCpyChild);

		/// <summary>
		/// Copies the origin from the FTPremade into this object, adding it to the scene
		/// </summary>
		/// <param name="premade : ">Premade to copy origin from.</param>
		/// <param name="scene : ">A scene object to add this object to.</param>
		Actor(FTPremade* premade, int id);

		virtual ~Actor();

	public:
		void AddChild(Actor* actor);
		void RemoveChild(Actor* actor);
		virtual void AddComponent(Component* component);
		void RemoveComponent(Component* component);
		void RemoveAllComponents();

	public:
		// Deep copies transform from another Actor.
		void CopyTransformFrom(Actor* actor);

		// Creates new Component with values from another Actor.
		void CopyComponentsFrom(Actor* actor);

		// Deep copies all child Actors
		virtual void CopyChildObjectFrom(Actor* actor);

		// Shallow copies all child Actors.
		void RefChildObjectFrom(Actor* actor);

	public:
		// Getters/Setters
		ActorGroup						GetActorGroup() const { return mActorGroup; }
		ActorGroup&						GetActorGroupRef() { return mActorGroup; }
		ActorGroup*						GetActorGroupPtr() { return &mActorGroup; }
		FTDS::String					GetName();
		virtual FTDS::String&			GetNameRef();
		const int						GetID() const { return mID; }
		const bool&						GetIsActive() const { return mIsActive; }
		Transform*						GetTransform() const { return mTransform; }
		Actor*							GetParent() const { return mParent; }
		FTDS::DynamicArray<Component*>* GetComponents() { return mComponents; }
		FTDS::DynamicArray<Actor*>*		GetChildActors() { return mChild; }
		const int&						GetDrawOrder() const { return mDrawOrder; }

		void SetName(FTDS::String&& name);
		void SetIsActive(bool isActive) { mIsActive = isActive; }
		void SetActorGroup(ActorGroup group) { mActorGroup = group; }
		void SetState(ActorState state) { mState = state; }
		void SetParent(Actor* parent) { mParent = parent; }
		void SetTransform(Transform* transform) { mTransform = transform; }
		void SetComponents(FTDS::DynamicArray<Component*>* components) { mComponents = components; }
		void SetChildActors(FTDS::DynamicArray<Actor*>* children) { mChild = children; }
		void SetDrawOrder(int order) { mDrawOrder = order; }

		bool		  HasName(FTDS::String&& name);
		bool		  HasName(const char* name);
		bool		  IsDead();
		virtual bool& IsActive();

		template <class T>
		T* GetComponent()
		{
			for (auto iter = mComponents.Begin(); iter != mComponents.End(); ++iter)
			{
				T* comp = dynamic_cast<T*>(*iter);
				if (comp)
					return comp;
			}
			return nullptr;
		};

	private:
		FTDS::String					mName;
		int								mID;
		ActorGroup						mActorGroup;
		ActorState						mState;
		bool							mIsActive;
		Transform*						mTransform;
		FTDS::DynamicArray<Component*>* mComponents;
		Actor*							mParent;
		FTDS::DynamicArray<Actor*>*		mChild;
		int								mDrawOrder;

	public:
		void SaveProperties(std::ofstream& ofs);
		void SaveComponents(std::ofstream& ofs);

		void LoadProperties(std::ifstream& ifs);
		void LoadComponents(std::ifstream& ifs);
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
} // namespace Core