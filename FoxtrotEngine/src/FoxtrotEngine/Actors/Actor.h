#pragma once
#include <vector>
#include <string>
#include <typeinfo>

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class Transform;
class SpriteComponent;
class ColliderComponent;
class KeyInputManager;
class Actor;
class UIActor;
class Scene;
class Component;
class FoxtrotRenderer;
enum class ACTOR_TAG;
enum class ACTOR_GROUP;

struct ActorData
{
	std::wstring			  mActorFrameName;
	ACTOR_GROUP				  mActorGroup;
	std::wstring			  mName;
	int						  mState;
	Transform*				  mTransform;
	std::vector<Component*>	  mComponents;
	Actor*					  mParent;
	std::vector<Actor*>	      mChild;
};

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
	ACTOR_GROUP	  GetActorGroup() const	  { return mActorGroup; }
	std::wstring  GetName()				  { return mName; }
	std::string   GetNameStr()			  { return ToString(mName); }
	std::wstring& GetNameRef()			  { return mName; }
	State		  GetState()	  const   { return mState; }
	std::string	  GetStateStr()	  const;
	State&		  GetStateRef()	  		  { return mState; }
	bool		  IsActive()	  const	  { return mState == State::EActive; }
	bool		  IsDead()		  const   { return mState == State::EDead; }
	Transform*    GetTransform()  const	  { return mTransform; }
	Actor*		  GetParent()	  const	  { return mParent; }
	std::vector<Component*>& GetComponents() { return mComponents; }
	std::vector<Actor*>&     GetChildActors() { return mChild; }
	unsigned int  GetID()		{ return mID; }
	static void   ResetNextID() { g_NextID = 0; }

	void		  SetName(std::wstring name)		{ mName = name; }
	void		  SetState(State state)				{ mState = state; }
	void		  SetState(std::string state);
	void		  SetActorGroup(ACTOR_GROUP group)	{ mActorGroup = group; }
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
			void UpdateComponents	 (float deltaTime);
			void LateUpdateComponents(float deltaTime);
			void RenderComponents	 (FoxtrotRenderer* renderer);

public:
	Actor(Scene* scene);
	// Copy Constructor
	Actor(Actor& origin);
	Actor(Actor* origin);
	virtual ~Actor();

	virtual Actor* Clone() { return nullptr; }

private:
	static int						g_NextID;
		   std::wstring				mName;
		   int						mID;
		   ACTOR_GROUP				mActorGroup;
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
	friend class Scene_Tool;
};