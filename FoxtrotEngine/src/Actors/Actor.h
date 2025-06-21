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
#include <vector>
#include <string>
#include <typeinfo>

#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "Actors/ActorGroup.h"

#ifdef FOXTROT_EDITOR
class EditorElement;
class EditorScene;
#endif // FOXTROT_EDITOR

class Transform;
class SpriteRenderer;
class Collider2D;
class FTInputDevice;
class UIActor;
class Scene;
class Component;
class FoxtrotRenderer;
class FTPremade;
enum class ACTOR_TAG;

class Actor
{
public:
	/// <summary>
	/// Creates an empty Actor with zero-initialized values, but doesn't add it to the Scene.
	/// </summary>
	Actor();

	/// <summary>
	/// Copy constructors that adds a deep-copied Actor to the scene.
	/// </summary>
	/// <param name="actor : "> Actor being copied.</param>
	Actor(Actor* actor);

	/// <summary>
	/// Copies the origin from the FTPremade into this object, adding it to the scene
	/// </summary>
	/// <param name="premade : ">Premade to copy origin from.</param>
	/// <param name="scene : ">A scene object to add this object to.</param>
	Actor(FTPremade* premade);

	virtual ~Actor();

public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};

public:
	void AddChild(Actor* actor);
	void RemoveChild(Actor* actor);
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
	void RemoveAllComponents();

public:
	// Deep copies transform from another Actor.
	void CopyTransformFrom(Actor* actor);

	// Creates new Component with values from another Actor.
	void CopyComponentsFrom(Actor* actor);

	// Deep copies all child Actors
	void CopyChildObjectFrom(Actor* actor);

	// Shallow copies all child Actors.
	void RefChildObjectFrom(Actor* actor);

public:
	// Getters/Setters
	ActorGroup				 GetActorGroup() const { return mActorGroup; }
	ActorGroup&				 GetActorGroupRef() { return mActorGroup; }
	ActorGroup*				 GetActorGroupPtr() { return &mActorGroup; }
	FTDS::String			 GetName() { return mName; }
	FTDS::String&			 GetNameRef() { return mName; }
	State					 GetState() const { return mState; }
	FTDS::String			 GetStateStr() const;
	State&					 GetStateRef() { return mState; }
	bool					 IsActive() const { return mState == State::EActive; }
	bool					 IsDead() const { return mState == State::EDead; }
	Transform*				 GetTransform() const { return mTransform; }
	Actor*					 GetParent() const { return mParent; }
	std::vector<Component*>& GetComponents() { return mComponents; }
	std::vector<Actor*>&	 GetChildActors() { return mChild; }
	const int&				 GetDrawOrder() const { return mDrawOrder; }

	void SetName(FTDS::String name) { mName = name; }
	void SetState(State state) { mState = state; }
	void SetState(FTDS::String state);
	void SetActorGroup(ActorGroup group) { mActorGroup = group; }
	void SetParent(Actor* parent) { mParent = parent; }
	void SetTransform(Transform* transform) { mTransform = transform; }
	void SetComponents(std::vector<Component*>& components) { mComponents = components; }
	void SetChildActors(std::vector<Actor*>& children) { mChild = children; }
	void SetDrawOrder(int order) { mDrawOrder = order; }

	bool HasName(FTDS::String& name);
	bool HasName(const char* name);

	template <class T>
	T* GetComponent()
	{
		for (int i = 0; i < mComponents.size(); i++)
		{
			if (typeid(*mComponents[i]) == typeid(T))
			{
				return dynamic_cast<T*>(mComponents[i]);
			}
		}
		return nullptr;
	};

public:
	virtual void Initialize(FTCore* coreInst);
	virtual void Setup();
	virtual void UpdateActor(float deltaTime) {};
	virtual void LateUpdateActor(float deltaTime) {};
	virtual void RenderActor(FoxtrotRenderer* renderer) {};

public:
	void		 ProcessInput(FTInputDevice* inputDevice);
	virtual void UpdateComponents(float deltaTime);
	void		 LateUpdateComponents(float deltaTime);
	virtual void RenderComponents(FoxtrotRenderer* renderer);

private:
	FTDS::String			mName;
	ActorGroup				mActorGroup;
	State					mState;
	Transform*				mTransform;
	std::vector<Component*> mComponents;
	Actor*					mParent;
	std::vector<Actor*>		mChild;
	int						mDrawOrder;

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
	constexpr const char* CHILD		 = "Child";
} // namespace ChunkKey