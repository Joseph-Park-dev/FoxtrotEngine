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
#include "Plugin/CoreExports.h"
#include "FTDS/Static/FTString.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Actor/IActor.h"

namespace Core
{
	class FTInputDevice;
	class FoxtrotRenderer;

	class Scene
	{
	public:
		/// @brief Adds an actor immediately or defers insertion while the scene is updating.
		/// @param actor Non-null heap-allocated actor; the scene takes ownership.
		/// @note Queues insertion during actor updates; the update flag is not a thread synchronization mechanism.
		CORE_API void AddActor(Common::IActor* actor);
		/// @brief Finds and deletes the actor from the scene's active or pending collection.
		/// @param actor Actor participating in this operation.
		/// @note Invalidates references to the removed actor.
		CORE_API void RemoveActor(Common::IActor* actor);

		/// @brief Searches the scene for an actor matching the supplied identifier or name.
		/// @param id Actor or entry identifier.
		/// @return Borrowed matching entry, or nullptr when the search does not find one.
		CORE_API Common::IActor*			FindActor(int id);
		/// @brief Searches the scene for an actor matching the supplied identifier or name.
		/// @param name Name used to identify the requested object or interface.
		/// @param filter Actor to exclude from the name search.
		/// @return Borrowed matching entry, or nullptr when the search does not find one.
		CORE_API virtual Common::IActor* FindActor(Common::FTDS::String& name, Common::IActor* filter = nullptr);
		/// @brief Searches the scene for an actor matching the supplied identifier or name.
		/// @param name Name used to identify the requested object or interface.
		/// @param filter Actor to exclude from the name search.
		/// @return Borrowed matching entry, or nullptr when the search does not find one.
		CORE_API Common::IActor*			FindActor(const char* name, Common::IActor* filter = nullptr);

	public:
		/// @brief Returns the name used by this scene.
		/// @return Borrowed access to the name.
		CORE_API const Common::FTDS::String&						   GetName();
		/// @brief Returns the is updating actors used by this scene.
		/// @return Current value of the is updating actors flag.
		CORE_API const bool										   GetIsUpdatingActors() const;
		/// @brief Returns the actors used by this scene.
		/// @return Borrowed access to the actors.
		CORE_API const Common::FTDS::DynamicArray<Common::IActor*>* GetActors() const;

		/// @brief Updates the name used by subsequent operations.
		/// @param name Replacement name.
		CORE_API void SetName(const Common::FTDS::String&& name);
		/// @brief Updates the is updating actors used by subsequent operations.
		/// @param value Replacement is updating actors.
		void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

		/// @brief Exposes the scene's mutable active-actor collection.
		/// @return Borrowed access to the actors.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API Common::FTDS::DynamicArray<Common::IActor*>*& Actors();

	public:
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		CORE_API void		 ProcessEvent();
		/// @brief Deletes the scene's managed actors and clears its collections.
		CORE_API virtual void DeleteAll();

	private:
		/// @brief Active actors managed by the scene.
		Common::FTDS::DynamicArray<Common::IActor*>* mActors;

		/// @brief Actors pending addition while updates are in progress.
		Common::FTDS::DynamicArray<Common::IActor*>* mPendingActors;

		/// @brief Human-readable scene name.
		Common::FTDS::String mSceneName;

		/// @brief True while the scene is iterating/updating actors.
		bool mIsUpdatingActors;

	public:
		/// @brief Constructs an empty scene.
		CORE_API Scene();

		/// @brief Destroys the scene and releases owned resources.
		CORE_API virtual ~Scene();

	private:
		/// @brief Commits pending actors to the active collection.
		/// @details
		/// Called after updates when it is safe to mutate actor arrays.
		CORE_API void AddPendingActors();

		/// @brief Removes and cleans up actors marked as dead.
		/// @details
		/// Executes after updates to keep collections consistent.
		CORE_API void ClearDeadActors();

		/// @brief Grants `EventManager` access to private event integration.
		friend class EventManager;
	};
} // namespace Core
