// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

/**
 * @file Scene.cpp
 * @brief Implements the `Scene` runtime container managing lifetime, update, and rendering of `Actor` instances.
 *
 * The `Scene` holds active and pending actors, coordinates their initialization, input processing,
 * update phases (Update/LateUpdate), and rendering. It also enforces insertion ordering by draw order
 * and performs event-driven actor addition/removal.
 *
 * Memory Ownership:
 * - The `Scene` owns `Actor` pointers stored in `mActors` and `mPendingActors`. It is responsible for deleting them.
 * - `DeleteAll()` releases all owned actors and clears arrays.
 *
 * Threading/Update Notes:
 * - `mIsUpdatingActors` guards against mutating the actor list while iterating during update. New actors are queued
 *   in `mPendingActors` and applied via `ProcessEvent()` -> `AddPendingActors()`.
 */

#include "Scenes/Scene.h"

#include <algorithm>

#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/SpriteRenderer.h"
#include "Components/UIs/PanelUI.h"
#include "Managers/EventManager.h"
#include "Core/TemplateFunctions.h"

#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
#endif // FOXTROT_EDITOR

/**
 * @brief Constructs a `Scene` with empty actor containers.
 *
 * Initializes the active and pending actor lists and sets updating flag to false.
 */
Scene::Scene()
	: mIsUpdatingActors(false)
	, mActors(DBG_NEW FTDS::DynamicArray<Actor*>)
	, mPendingActors(DBG_NEW FTDS::DynamicArray<Actor*>)
{
}

/**
 * @brief Destructor. Cleans up all actors and internal containers.
 *
 * Calls `DeleteAll()` to delete owned actors, then deletes container instances.
 */
Scene::~Scene()
{
	DeleteAll();
	delete mActors;
	delete mPendingActors;
}

/**
 * @brief Finds an actor by unique identifier.
 * @param id Unique actor ID to search for.
 * @return Pointer to the actor if found; otherwise `nullptr`.
 */
Actor* Scene::FindActor(int id)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->GetID() == id)
			return (*iter);
		else
			continue;
	}
	return nullptr;
}

/**
 * @brief Finds an actor by name with optional filter to exclude a specific actor.
 * @param name Actor name to search for.
 * @param filter Optional actor pointer to exclude from match (commonly the caller).
 * @return Pointer to the actor if found; otherwise `nullptr`.
 */
Actor* Scene::FindActor(FTDS::String& name, Actor* filter)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->GetName().Equal(name.C_Str()))
			if ((*iter) != filter)
				return (*iter);
			else
				continue;
		else
			continue;
	}
	return nullptr;
}

/**
 * @brief Overload for finding an actor by C-string name with optional filter.
 * @param name Null-terminated string name.
 * @param filter Optional actor pointer to exclude.
 * @return Pointer to the actor if found; otherwise `nullptr`.
 */
Actor* Scene::FindActor(const char* name, Actor* filter)
{
	FTDS::String str(name);
	return FindActor(str, filter);
}

/**
 * @brief Indicates if the scene is currently iterating/updating actors.
 * @return True if within `Update()` loop; otherwise false.
 */
const bool Scene::GetIsUpdatingActors() const
{
	return mIsUpdatingActors;
}

/**
 * @brief Gets a const pointer to the active actors container.
 * @return Const pointer to `FTDS::DynamicArray<Actor*>`.
 *
 * Note: Do not mutate actors via this container during update; use event processing.
 */
const FTDS::DynamicArray<Actor*>* Scene::GetActors() const
{
	return mActors;
}

/**
 * @brief Provides a reference to the active actors container pointer.
 * @return Reference to `FTDS::DynamicArray<Actor*>*` for advanced management.
 *
 * Warning: Ownership and lifetime are managed internally; avoid replacing the container unless intended.
 */
FTDS::DynamicArray<Actor*>*& Scene::Actors()
{
	return mActors;
}

/**
 * @brief Initializes all actors in the scene.
 * @param coreInst Core engine instance passed to actors.
 *
 * Calls each actor's `Initialize()` regardless of activation state.
 */
void Scene::Initialize(FTCore* coreInst)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		(*iter)->Initialize(coreInst);
}

/**
 * @brief Performs setup for all actors in the scene.
 *
 * Calls each actor's `Setup()` to finalize construction/configuration.
 */
void Scene::Setup()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		(*iter)->Setup();
}

/**
 * @brief Forwards input to active actors.
 * @param inputDevice Input device handle to process.
 *
 * Only active actors receive input events.
 */
void Scene::ProcessInput(FTInputDevice* inputDevice)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		if ((*iter)->IsActive())
			(*iter)->ProcessInput(inputDevice);
}

/**
 * @brief Updates active actors and their components.
 * @param deltaTime Time step in seconds.
 *
 * Sets `mIsUpdatingActors` to guard against concurrent modifications. Actors
 * receive component updates followed by actor logic updates.
 */
void Scene::Update(float deltaTime)
{
	mIsUpdatingActors = true;
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->UpdateComponents(deltaTime);
			(*iter)->UpdateActor(deltaTime);
		}
	}
	mIsUpdatingActors = false;
}

/**
 * @brief Executes late update pass for active actors and their components.
 * @param deltaTime Time step in seconds.
 *
 * Intended for logic requiring completion of `Update()` (ordering-sensitive).
 */
void Scene::LateUpdate(float deltaTime)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->LateUpdateComponents(deltaTime);
			(*iter)->LateUpdateActor(deltaTime);
		}
	}
}

/**
 * @brief Renders active actors and their components.
 * @param renderer Rendering context.
 *
 * Calls component rendering first, then actor-specific rendering.
 */
void Scene::Render(FoxtrotRenderer* renderer)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->RenderComponents(renderer);
			(*iter)->RenderActor(renderer);
		}
	}
}

/**
 * @brief Adds an actor to the scene.
 * @param actor Newly created actor pointer (owned by the scene after addition).
 *
 * Behavior:
 * - If currently updating, actor is queued in `mPendingActors`.
 * - Otherwise, actor is inserted into `mActors` ordered by `GetDrawOrder()`.
 *   Lower draw order actors are placed earlier.
 *
 * Precondition: `actor` must be non-null and allocated on heap.
 */
void Scene::AddActor(Actor* actor)
{
	if (mIsUpdatingActors)
		mPendingActors->PushBack(actor);
	else
	{
		if (!mActors->IsEmpty())
		{
			int	 drawOrder = actor->GetDrawOrder();
			auto iter	   = mActors->Begin();
			for (; iter != mActors->End(); ++iter)
			{
				if (*iter)
					if (drawOrder < (*iter)->GetDrawOrder())
						break;
			}
			mActors->Insert(iter.IterPos(), actor);
		}
		else
			mActors->PushBack(actor);
	}
}

/**
 * @brief Processes scene-level events: applies pending additions and removes dead actors.
 *
 * Typical call site: once per frame after updates.
 */
void Scene::ProcessEvent()
{
	AddPendingActors();
	ClearDeadActors();
}

/**
 * @brief Deletes all actors (active and pending) and clears containers.
 *
 * Safe to call multiple times. Sets deleted pointer slots to `nullptr` before clearing.
 */
void Scene::DeleteAll()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}

	for (auto iter = mPendingActors->Begin(); iter != mPendingActors->End(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}
	mActors->Clear();
	mPendingActors->Clear();
}

/**
 * @brief Moves all pending actors into the active list.
 *
 * Note: Current implementation clears `mPendingActors` inside the loop; the clear
 * empties the container immediately after first push. If multiple actors are pending,
 * consider deferring the clear until after the loop to add all items.
 */
void Scene::AddPendingActors()
{
	for (auto iter = mPendingActors->Begin(); iter != mPendingActors->End(); ++iter)
	{
		mActors->PushBack(*iter);
		mPendingActors->Clear();
	}
}

/**
 * @brief Removes actors flagged as dead from the scene.
 *
 * Iterates through active actors and calls `RemoveActor()` for those reporting `IsDead()`.
 */
void Scene::ClearDeadActors()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if (!(*iter)->GetIsActive())
			RemoveActor((*iter));
	}
}

/**
 * @brief Removes and deletes an actor from either pending or active containers.
 * @param actor Actor pointer to remove (owned by the scene).
 *
 * Behavior:
 * - Searches and deletes from `mPendingActors` if present, then erases entry.
 * - Searches and deletes from `mActors` if present, then erases entry.
 *
 * Postcondition: If found, actor memory is freed and removed from container.
 */
void Scene::RemoveActor(Actor* actor)
{
	int pos = -1;

	pos = mPendingActors->Find(actor);
	if (pos != -1)
	{
		delete mPendingActors->At(pos);
		mPendingActors->Erase(pos);
	}

	pos = mActors->Find(actor);
	if (pos != -1)
	{
		delete mActors->At(pos);
		mActors->Erase(pos);
	}
}