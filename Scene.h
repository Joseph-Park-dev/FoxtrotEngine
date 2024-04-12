#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Actor;
enum class ACTOR_GROUP;
class CCore;
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
		return mActors[(Uint32)group];
	};
	std::vector<Actor*>& GetActorGroup(Uint32 group)
	{
		return mActors[group];
	};
	void SetName(const std::wstring& name) { mSceneName = name; }

public:
	void		 ProcessInput(KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	void		 LateUpdate(float deltaTime);
	void		 Render(SDL_Renderer* renderer);
	void		 ProcessEvent();
	void		 DeleteAll();

private:
	std::vector<Actor*> mActors[(Uint32)ACTOR_GROUP::END];
	std::vector<Actor*> mPendingActors[(Uint32)ACTOR_GROUP::END];
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