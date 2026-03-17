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
#include "static/FTString.h"

class Actor;
class FTCore;
class SpriteRendererComponent;
class FTInputDevice;
class FoxtrotRenderer;
namespace FTDS
{
	class String;
	template <typename TYPE>
	class DynamicArray;
}

class Scene
{
public:
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	Actor* FindActor(int id);
	Actor* FindActor(FTDS::String& name, Actor* filter = nullptr);
	Actor* FindActor(const char* name, Actor* filter = nullptr);

public:
	const FTDS::String&				  GetName();
	const bool						  GetIsUpdatingActors() const;
	const FTDS::DynamicArray<Actor*>* GetActors() const;

	void SetName(const FTDS::String&& name);
	void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

	FTDS::DynamicArray<Actor*>*& Actors();

public:
	void		 ProcessEvent();
	virtual void DeleteAll();

private:
	/// @brief Active actors managed by the scene.
	FTDS::DynamicArray<Actor*>* mActors;

	/// @brief Actors pending addition while updates are in progress.
	FTDS::DynamicArray<Actor*>* mPendingActors;

	/// @brief Human-readable scene name.
	FTDS::String mSceneName;

	/// @brief True while the scene is iterating/updating actors.
	bool mIsUpdatingActors;

public:
	/// @brief Constructs an empty scene.
	Scene();

	/// @brief Destroys the scene and releases owned resources.
	virtual ~Scene();

private:
	/// @brief Commits pending actors to the active collection.
	/// @details
	/// Called after updates when it is safe to mutate actor arrays.
	void AddPendingActors();

	/// @brief Removes and cleans up actors marked as dead.
	/// @details
	/// Executes after updates to keep collections consistent.
	void ClearDeadActors();

	/// @brief Grants `EventManager` access to private event integration.
	friend class EventManager;
};