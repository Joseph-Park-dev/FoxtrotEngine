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

/// @brief Base class representing a collection of actors and their lifecycle within a scene.
/// @details
/// A `Scene` owns and manages `Actor` instances. It controls initialization,
/// input processing, updating, late updating, rendering, and event processing.
/// Actors can be staged while updates are in progress and committed later to avoid
/// mutation during iteration. `Scene` is managed externally by the SceneManager.
class Scene
{
public:
	/// @brief Adds an actor to the scene.
	/// @details
	/// If the scene is currently updating actors (`GetIsUpdatingActors()` is true),
	/// the actor is added to a pending list and will be committed when safe.
	/// Otherwise, the actor is added to the active actors list immediately.
	/// @param actor Non-null pointer to an `Actor` to add. Ownership is managed by the scene.
	void AddActor(Actor* actor);

	/// @brief Removes an actor from the scene.
	/// @details
	/// Marks the actor for removal and clears it from the active/pending collections
	/// during cleanup. Safe to call while updating; removal is deferred until a
	/// cleanup pass.
	/// @param actor Pointer to the `Actor` to remove. If not found, the call is a no-op.
	void RemoveActor(Actor* actor);

	/// @brief Finds an actor by unique ID using a linear search through the hierarchy.
	/// @param id The unique actor ID to search for.
	/// @return Pointer to the matching `Actor` if found; otherwise `nullptr`.
	Actor* FindActor(int id);

	/// @brief Finds an actor by name using a linear search.
	/// @details
	/// Optionally provide a filter actor (e.g., a parent or owner) to limit matches.
	/// @param name The name to match.
	/// @param filter Optional actor used to filter matches (nullptr for no filter).
	/// @return Pointer to the first matching `Actor` if found; otherwise `nullptr`.
	Actor* FindActor(FTDS::String& name, Actor* filter = nullptr);

	/// @brief Finds an actor by C-string name using a linear search.
	/// @details
	/// Optionally provide a filter actor (e.g., a parent or owner) to limit matches.
	/// @param name Null-terminated C-string actor name to match.
	/// @param filter Optional actor used to filter matches (nullptr for no filter).
	/// @return Pointer to the first matching `Actor` if found; otherwise `nullptr`.
	Actor* FindActor(const char* name, Actor* filter = nullptr);

public:
	/// @brief Gets the scene name.
	/// @return Constant reference to a wide string containing the scene name.
	const std::wstring& GetName() { return mSceneName; }

	/// @brief Indicates whether actors are currently being updated.
	/// @details
	/// When true, modifications to the actor collection are deferred.
	/// @return True if the scene is iterating/updating actors; otherwise false.
	const bool GetIsUpdatingActors() const;

	/// @brief Gets the active actor collection.
	/// @return Constant pointer to the dynamic array of active actors.
	const FTDS::DynamicArray<Actor*>* GetActors() const;

	/// @brief Sets the scene name.
	/// @param name New wide-string name of the scene.
	void SetName(const std::wstring& name) { mSceneName = name; }

	/// @brief Sets whether the scene is currently updating actors.
	/// @param value True to mark updating, false to mark idle.
	void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

	/// @brief Gets a modifiable reference to the active actor array pointer.
	/// @details
	/// Use with care. Prefer `AddActor`/`RemoveActor` for managing actor lifecycle.
	/// @return Reference to the pointer of the active actors array.
	FTDS::DynamicArray<Actor*>*& Actors();

public:
	/// @brief Initializes the scene using the provided core instance.
	/// @details
	/// Allocates internal collections and prepares the scene for operation.
	/// @param coreInst Pointer to the engine core instance.
	void Initialize(FTCore* coreInst);

	/// @brief Performs one-time setup for the scene content.
	/// @details
	/// Override to construct initial actors, components, and resources.
	virtual void Setup();

	/// @brief Processes input for the scene.
	/// @details
	/// Called every frame prior to update. Override to handle player input.
	/// @param inputDevice Pointer to the input device abstraction.
	virtual void ProcessInput(FTInputDevice* inputDevice);

	/// @brief Updates the scene and its actors.
	/// @details
	/// Called once per frame. Override to implement per-frame logic.
	/// @param deltaTime Elapsed time in seconds since last frame.
	virtual void Update(float deltaTime);

	/// @brief Performs late update on the scene and its actors.
	/// @details
	/// Called after `Update`. Override to handle order-dependent adjustments.
	/// @param deltaTime Elapsed time in seconds since last frame.
	virtual void LateUpdate(float deltaTime);

	/// @brief Renders the scene using the provided renderer.
	/// @details
	/// Override to submit draw calls and render actors/components.
	/// @param renderer Pointer to the rendering system.
	virtual void Render(FoxtrotRenderer* renderer);

	/// @brief Processes queued events for the scene.
	/// @details
	/// Integrates with the `EventManager` to deliver pending events to actors.
	void ProcessEvent();

	/// @brief Deletes all actors and clears scene state.
	/// @details
	/// Override to perform additional cleanup. Base implementation ensures
	/// all actors and pending actors are released.
	virtual void DeleteAll();

private:
	/// @brief Active actors managed by the scene.
	FTDS::DynamicArray<Actor*>* mActors;

	/// @brief Actors pending addition while updates are in progress.
	FTDS::DynamicArray<Actor*>* mPendingActors;

	/// @brief Human-readable scene name.
	std::wstring mSceneName;

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