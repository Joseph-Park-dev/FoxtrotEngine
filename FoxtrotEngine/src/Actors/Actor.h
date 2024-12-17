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
class SpriteRendererComponent;
class Collider2DComponent;
class KeyInputManager;
class UIActor;
class Scene;
class Component;
class FoxtrotRenderer;
enum class ACTOR_TAG;

class Actor
{
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
	void CopyTransformFrom(Actor& origin);
	void CopyComponentsFrom(Actor& origin);
	void CopyChildObject(Actor& origin);

	void CopyTransformFrom(Actor* origin);
	void CopyComponentsFrom(Actor* origin);
	void CopyChildObject(Actor* origin);

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
	UINT		  GetID()		  const			{ return mID; }

	void		  SetName		 (std::string name)						{ mName = name; }
	void		  SetState		 (State state)							{ mState = state; }
	void		  SetState		 (std::string state);
	void		  SetActorGroup	 (ActorGroup group)						{ mActorGroup = group; }
	void		  SetParent		 (Actor* parent)						{ mParent = parent; }
	void		  SetTransform	 (Transform* transform)					{ mTransform = transform; }
	void		  SetComponents	 (std::vector<Component*>& components)	{ mComponents = components; }
	void		  SetChildActors (std::vector<Actor*>& children)		{ mChild = children; }

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
	virtual void UpdateActor	(float deltaTime)	{};
	virtual void LateUpdateActor(float deltaTime)	{};
	virtual void RenderActor	(FoxtrotRenderer* renderer){};

public:
			void ProcessInput		 (KeyInputManager* keyInputManager);
	virtual void UpdateComponents	 (float deltaTime);
			void LateUpdateComponents(float deltaTime);
	virtual void RenderComponents	 (FoxtrotRenderer* renderer);

public:
	// Constructor used for FTPremade origin,
	// not recommended to use outside of FTPremade
	Actor();
	Actor(Actor* origin); 

	// Use this when adding Actors to a scene.
	Actor(Scene* scene);
	// Copy Constructors
	Actor(Actor& origin, Scene* scene);
	Actor(Actor* origin, Scene* scene);
	virtual ~Actor();

	virtual void CloneTo(Actor* target);

protected:
	virtual void OnCollisionEnter(Collider2DComponent* _pOther);
	virtual void OnCollisionStay(Collider2DComponent* _pOther);
	virtual void OnCollisionExit(Collider2DComponent* _pOther);
	virtual void OnRayEnter();
	friend class Collider2DComponent;

private:
	std::string				mName;
	int						mID;
	ActorGroup				mActorGroup;
	State					mState;
	Transform*				mTransform;
	std::vector<Component*>	mComponents;
	Actor*					mParent;
	std::vector<Actor*>		mChild;

private:
	bool CheckCompInitialized();

public:
	void SaveProperties(std::ofstream& ofs);
	void SaveComponents(std::ofstream& ofs);

	void LoadProperties(std::ifstream& ifs);
	void LoadComponents(std::ifstream& ifs);
};