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
class FTInputDevice;
class FoxtrotRenderer;

// Base class that every scene object is derived from
class Scene
{
public:
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	/// <summary>
	/// Finds the Actor from current scene by linear-searching the hierarchy.
	/// </summary>
	/// <param name="group : ">ActorGroup this Actor is assigned to.</param>
	/// <param name="name : ">Actor's name</param>
	/// <returns></returns>
	Actor* FindActor(FTDS::String& name, Actor* filter = nullptr);
	Actor* FindActor(const char* name, Actor* filter = nullptr);

public:
	const std::wstring&				  GetName() { return mSceneName; }
	const FTDS::DynamicArray<Actor*>* GetActors() const;

	void SetName(const std::wstring& name) { mSceneName = name; }
	void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

	FTDS::DynamicArray<Actor*>*& Actors();

public:
	void		 Initialize(FTCore* coreInst);
	virtual void Setup();
	virtual void ProcessInput(FTInputDevice* inputDevice);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);
	void		 ProcessEvent();
	virtual void DeleteAll();

private:
	FTDS::DynamicArray<Actor*>* mActors;
	FTDS::DynamicArray<Actor*>* mPendingActors;
	std::wstring				mSceneName;
	bool						mIsUpdatingActors;

public:
	Scene();
	virtual ~Scene();

private:
	void AddPendingActors();
	void ClearDeadActors();

	friend class EventManager;
};