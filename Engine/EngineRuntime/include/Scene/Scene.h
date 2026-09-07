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

	class CORE_API Scene
	{
	public:
		void AddActor(Common::IActor* actor);
		void RemoveActor(Common::IActor* actor);

		Common::IActor*			FindActor(int id);
		virtual Common::IActor* FindActor(Common::FTDS::String& name, Common::IActor* filter = nullptr);
		Common::IActor*			FindActor(const char* name, Common::IActor* filter = nullptr);

	public:
		const Common::FTDS::String&						   GetName();
		const bool										   GetIsUpdatingActors() const;
		const Common::FTDS::DynamicArray<Common::IActor*>* GetActors() const;

		void SetName(const Common::FTDS::String&& name);
		void SetIsUpdatingActors(bool value) { mIsUpdatingActors = value; }

		Common::FTDS::DynamicArray<Common::IActor*>*& Actors();

	public:
		void		 ProcessEvent();
		virtual void DeleteAll();

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
} // namespace Core