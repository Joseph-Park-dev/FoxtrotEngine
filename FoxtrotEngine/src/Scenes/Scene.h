// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// An object that the Actors are placed, and interacts with each others.
/// Managed by the SceneManager.
/// </summary>

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

	/// <summary>
	/// Finds the Actor from current scene by linear-searching the hierarchy.
	/// </summary>
	/// <param name="group : ">ActorGroup this Actor is assigned to.</param>
	/// <param name="name : ">Actor's name</param>
	/// <returns></returns>
	Actor* FindActor(std::string& name);
	Actor* FindActor(const char* name);

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
	std::vector<Actor*>* GetActors() {
		return mActors;
	}
	size_t GetActorCount()
	{
		size_t size = 0;
		for (size_t i = 0; i < size_t(ActorGroup::END); ++i) {
			size += GetActorGroup(i).size();
		}
		return size;
	};

	void SetName(const std::wstring& name) { mSceneName = name; }
	void SetIsUpdatingActors(bool value){ mIsUpdatingActors = value; }

public:
	virtual void Setup();
	virtual void ProcessInput(KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);
			void ProcessEvent();
	virtual void DeleteAll();

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
};