#pragma once
#include <vector>
#include <string>
#include <typeinfo>
#include <nlohmann/json.hpp>

#include "Core/TemplateFunctions.h"
#include "Actors/ActorGroup.h"

class Transform;
class SpriteRendererComponent;
class ColliderComponent;
class KeyInputManager;
class Actor;
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
	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

protected:
	virtual void OnCollisionEnter(ColliderComponent* _pOther);
	virtual void OnCollisionStay(ColliderComponent* _pOther);
	virtual void OnCollisionExit(ColliderComponent* _pOther);
	virtual void OnRayEnter();
	friend class ColliderComponent;

public:
	// Getters/Setters
	ActorGroup	  GetActorGroup() const	  { return mActorGroup; }
	std::string   GetName()				  { return mName; }
	std::string&  GetNameRef()			  { return mName; }
	State		  GetState()	  const   { return mState; }
	std::string	  GetStateStr()	  const;
	State&		  GetStateRef()	  		  { return mState; }
	bool		  IsActive()	  const	  { return mState == State::EActive; }
	bool		  IsDead()		  const   { return mState == State::EDead; }
	Transform*    GetTransform()  const	  { return mTransform; }
	Actor*		  GetParent()	  const	  { return mParent; }
	std::vector<Component*>& GetComponents()  { return mComponents; }
	std::vector<Actor*>&     GetChildActors() { return mChild; }
	unsigned int  GetID()		{ return mID; }
	static void   ResetNextID() { g_NextID = 0; }

	void		  SetName(std::string name)			{ mName = name; }
	void		  SetState(State state)				{ mState = state; }
	void		  SetState(std::string state);
	void		  SetActorGroup(ActorGroup group)	{ mActorGroup = group; }
	void		  SetParent(Actor* parent)			{ mParent = parent; }
	void		  SetTransform(Transform* transform){ mTransform = transform; }
	// Shallow copy Component objects -> deep copy pointers
	void		  SetComponents(std::vector<Component*>& components) { mComponents = components; }
	void		  SetChildActors(std::vector<Actor*>& children)  { mChild = children; }

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

protected:
	void CopyTransformFrom(Actor& origin);
	void CopyComponentsFrom(Actor& origin);

	void CopyTransformFrom(Actor* origin);
	void CopyComponentsFrom(Actor* origin);

public:
	virtual void Initialize		()					{};
	virtual void UpdateActor	(float deltaTime)	{};
	virtual void LateUpdateActor(float deltaTime)	{};
	virtual void RenderActor	(FoxtrotRenderer* renderer){};

public:
			void ProcessInput		 (KeyInputManager* keyInputManager);
	virtual void UpdateComponents	 (float deltaTime);
			void LateUpdateComponents(float deltaTime);
	virtual void RenderComponents	 (FoxtrotRenderer* renderer);

public:
	Actor(Scene* scene);
	// Copy Constructor
	Actor(Actor& origin, Scene* scene);
	Actor(Actor* origin, Scene* scene);
	virtual ~Actor();

	virtual Actor* Clone() { return nullptr; }

private:
	static int						g_NextID;
		   std::string				mName;
		   int						mID;
		   ActorGroup	mActorGroup;
		   State					mState;
		   Transform*				mTransform;
		   std::vector<Component*>	mComponents;
		   Actor*					mParent;
		   std::vector<Actor*>		mChild;

public:
	void LoadProperties(std::ifstream& ifs);
	void LoadComponents(std::ifstream& ifs);

	void LoadProperties(nlohmann::ordered_json& ifs);
	void LoadComponents(nlohmann::ordered_json& ifs);

	void SaveProperties(std::ofstream& ofs);
	void SaveComponents(std::ofstream& ofs);

#ifdef FOXTROT_EDITOR
public:
	void SaveProperties(nlohmann::ordered_json& out);
	void SaveComponents(nlohmann::ordered_json& out);
#endif //  FOXTROT_EDITOR
};