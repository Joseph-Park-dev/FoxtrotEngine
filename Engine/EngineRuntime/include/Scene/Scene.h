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
		CORE_API void AddActor(Common::IActor* actor);
		CORE_API void RemoveActor(Common::IActor* actor);

		CORE_API Common::IActor*			FindActor(int id);
		CORE_API virtual Common::IActor* FindActor(Common::FTDS::String& name, Common::IActor* filter = nullptr);
		CORE_API Common::IActor*			FindActor(const char* name, Common::IActor* filter = nullptr);

	public:
		CORE_API const Common::FTDS::String&						   GetName();
		CORE_API const bool										   GetIsUpdatingActors() const;
		CORE_API const Common::FTDS::DynamicArray<Common::IActor*>* GetActors() const;

		CORE_API void SetName(const Common::FTDS::String&& name);
		void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

		CORE_API Common::FTDS::DynamicArray<Common::IActor*>*& Actors();

	public:
		CORE_API void		 ProcessEvent();
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