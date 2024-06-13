#pragma once
#include <string>
#include <vector>

#include "ActorGroup.h"

class Actor;
class FTCore;
class SpriteComponent;
class KeyInputManager;

// Base class that every scene object is derived from
class Scene
{
public:
	void AddActor(Actor* actor, ACTOR_GROUP group);

public:
	virtual void Enter() = 0;
	virtual void Exit() = 0;

protected:
	virtual void LoadData() = 0;

public:
	const std::wstring&  GetName() { return mSceneName; }
	std::vector<Actor*>& GetActorGroup(ACTOR_GROUP group)
	{
		return mActors[(size_t)group];
	};
	std::vector<Actor*>& GetActorGroup(size_t group)
	{
		return mActors[group];
	};
	void SetName(const std::wstring& name) { mSceneName = name; }

public:
	void		 ProcessInput(KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	void		 LateUpdate(float deltaTime);
	void		 Render(FoxtrotRenderer* renderer);
	void		 ProcessEvent();
	void		 DeleteAll();

private:
	std::vector<Actor*> mActors[(size_t)ACTOR_GROUP::END];
	std::vector<Actor*> mPendingActors[(size_t)ACTOR_GROUP::END];
	std::wstring		mSceneName;
	bool				mIsUpdatingActors;

public:
	Scene();
	virtual ~Scene();

private:
	void AddPendingActors();
	void ClearDeadActors();
	void RemoveActor(Actor* actor);
	void DeleteGroup(ACTOR_GROUP group);
	void DeletePendingGroup(ACTOR_GROUP group);

	friend class EventManager;

#ifdef _DEBUG
public:
	void EditorUpdate(float deltaTime);
	void EditorLateUpdate(float deltaTime);
#endif
};