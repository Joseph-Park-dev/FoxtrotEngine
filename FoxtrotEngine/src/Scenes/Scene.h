#pragma once
#include <string>
#include <vector>

#include "Actors/ActorGroup.h"

class Actor;
class FTCore;
class SpriteRendererComponent;
class KeyInputManager;
class FoxtrotRenderer;

// Base class that every scene object is derived from
class Scene
{
public:
	void AddActor(Actor* actor, ActorGroup group);

public:
	virtual void Enter() = 0;
	virtual void Exit() = 0;

protected:
	virtual void LoadData() = 0;

public:
	const std::wstring&  GetName() { return mSceneName; }
	std::vector<Actor*>& GetActorGroup(ActorGroup group)
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
	std::vector<Actor*> mActors[(size_t)ActorGroup::END];
	std::vector<Actor*> mPendingActors[(size_t)ActorGroup::END];
	std::wstring		mSceneName;
	bool				mIsUpdatingActors;

public:
	Scene();
	virtual ~Scene();

private:
	void AddPendingActors();
	void ClearDeadActors();
	void RemoveActor(Actor* actor);
	void DeleteGroup(ActorGroup group);
	void DeletePendingGroup(ActorGroup group);

	friend class EventManager;


public:
	void EditorProcessInput(KeyInputManager* keyInputManager);
	void EditorUpdate(float deltaTime);
	void EditorLateUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

};