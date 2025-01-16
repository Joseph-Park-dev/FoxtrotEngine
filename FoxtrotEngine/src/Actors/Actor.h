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
#endif // FOXTROT_EDITOR

class Transform;
class SpriteRenderer;
class Collider2D;
class KeyInputManager;
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
	/// Constructor that adds an Actor to scene during initialization phase.
	/// (When a .chunk is being loaded)
	/// </summary>
	/// <param name="scene : "> A scene object which this Actor is loaded to.</param>
	Actor(Scene* scene);

	/// <summary>
	/// Creates an deep-copied Actor from another Actor Obj, or an Origin from Premade.
	/// This doesn't add the created Actor to the scene, but stores the initialized one.
	/// </summary>
	/// <param name="actor : "> Actor being copied.</param>
	Actor(Actor* actor);

	/// <summary>
	/// Copy constructors that adds a deep-copied Actor to the scene.
	/// </summary>
	/// <param name="actor : "> Actor being copied.</param>
	/// <param name="scene : "> A scene object which this Actor is loaded to.</param>
	Actor(Actor* actor, Scene* scene);

	/// <summary>
	/// Copies the origin from the FTPremade into this object, adding it to the scene
	/// </summary>
	/// <param name="premade : ">Premade to copy origin from.</param>
	/// <param name="scene : ">A scene object to add this object to.</param>
	Actor(FTPremade* premade, Scene* scene);

	virtual ~Actor();

public:
	enum State
	{
		EActive, EPaused, EDead
	};

public:
	void AddChild(Actor* actor) 
	{ 
		actor->SetParent(this);
		mChild.emplace_back(actor);
	}
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
	void RemoveAllComponents();

public:
	// Deep copies transform from another Actor.
	void CopyTransformFrom(Actor* actor);

	// Creates new Component with values from another Actor.
	void CopyComponentsFrom(Actor* actor);

	// This method has problems in the current version
	// : recursive calling
	void CopyChildObjectFrom(Actor* actor);

public:
	// Getters/Setters
	ActorGroup	  GetActorGroup()	 const		{ return mActorGroup; }
	ActorGroup*	  GetActorGroupPtr()			{ return &mActorGroup; }
	std::string   GetName()		  const			{ return mName; }
	std::string&  GetNameRef()					{ return mName; }
	State		  GetState()	  const			{ return mState; }
	std::string	  GetStateStr()	  const;
	State&		  GetStateRef()	  				{ return mState; }
	bool		  IsActive()	  const			{ return mState == State::EActive; }
	bool		  IsDead()		  const			{ return mState == State::EDead; }
	Transform*    GetTransform()  const			{ return mTransform; }
	Actor*		  GetParent()	  const			{ return mParent; }
	std::vector<Component*>& GetComponents()	{ return mComponents; }
	std::vector<Actor*>&     GetChildActors()	{ return mChild; }

	void		  SetName		 (std::string name)						{ mName = name; }
	void		  SetState		 (State state)							{ mState = state; }
	void		  SetState		 (std::string state);
	void		  SetActorGroup	 (ActorGroup group)						{ mActorGroup = group; }
	void		  SetParent		 (Actor* parent)						{ mParent = parent; }
	void		  SetTransform	 (Transform* transform)					{ mTransform = transform; }
	void		  SetComponents	 (std::vector<Component*>& components)	{ mComponents = components; }
	void		  SetChildActors (std::vector<Actor*>& children)		{ mChild = children; }

	bool		  HasName		 (std::string& name);
	bool		  HasName		 (const char* name);

	template<class T>
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
	virtual void Initialize		(FTCore* coreInst);
	virtual void Setup			();
	virtual void UpdateActor	(float deltaTime)			{};
	virtual void LateUpdateActor(float deltaTime)			{};
	virtual void RenderActor	(FoxtrotRenderer* renderer)	{};

public:
			void ProcessInput		 (KeyInputManager* keyInputManager);
	virtual void UpdateComponents	 (float deltaTime);
			void LateUpdateComponents(float deltaTime);
	virtual void RenderComponents	 (FoxtrotRenderer* renderer);

protected:
	virtual void OnCollisionEnter(Collider2D* _pOther);
	virtual void OnCollisionStay(Collider2D* _pOther);
	virtual void OnCollisionExit(Collider2D* _pOther);
	virtual void OnRayEnter();
	friend class Collider2D;

private:
	std::string				mName;
	ActorGroup				mActorGroup;
	State					mState;
	Transform*				mTransform;
	std::vector<Component*>	mComponents;
	Actor*					mParent;
	std::vector<Actor*>		mChild;

public:
	void SaveProperties(std::ofstream& ofs);
	void SaveComponents(std::ofstream& ofs);

	void LoadProperties(std::ifstream& ifs);
	void LoadComponents(std::ifstream& ifs);
};