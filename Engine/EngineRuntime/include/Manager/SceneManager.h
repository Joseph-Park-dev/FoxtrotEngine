// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton manager that holds a Scene instance, switches a Scene,
/// and runs the Scene as a part of the game loop.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "Entity/Entity.h"

#include <iosfwd>

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"

#include "Plugin/CoreExports.h"

namespace Common
{
	class IActor;
	namespace FTDS
	{
		template <typename TYPE>
		class DynamicArray;
	} // namespace FTDS
} // namespace Common

namespace Core
{
	class Scene;

#ifndef FOXTROT_EDITOR // If it's not on Editor.
	#define FIND_ACTOR_FILTER(name, filter) SceneManager::GetInstance()->GetCurrentScene()->FindActor(name, filter)
	#define FIND_ACTOR(id) SceneManager::GetInstance()->GetCurrentScene()->FindActor(id)
#endif // FOXTROT_EDITOR

	class SceneManager
	{
	public:
		/// @brief Returns the shared instance used by this manager.
		/// @return Borrowed singleton pointer; do not delete it directly.
		/// @note Synchronize concurrent initialization, access, and destruction externally.
		CORE_API static SceneManager* GetInstance();
		/// @brief Deletes the shared instance and clears the singleton pointer.
		/// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
		CORE_API static void Destroy();
		/// @brief Disables copying so the instance's managed state cannot be duplicated.
		/// @note Unnamed parameter (const SceneManager&): reserved by this interface or unused by this implementation.
		SceneManager(const SceneManager&) = delete;
	protected:
		/// @brief Initializes scene selection and chunk-list state.
		CORE_API SceneManager();
		/// @brief Releases the resources managed by this instance during destruction.
		CORE_API ~SceneManager();
	private:
		static SceneManager* mInstance;

	public:
		/// @brief Selects and loads a scene from the configured chunk list.
		/// @param index Zero-based element index.
		CORE_API void		   SwitchScene(size_t index);
		/// @brief Returns the current scene used by this scene manager.
		/// @return Borrowed access to the current scene.
		CORE_API virtual Scene* GetCurrentScene();

	public:
		/// @brief Updates the chunk list path used by subsequent operations.
		/// @param path Replacement chunk list path.
		CORE_API void SetChunkListPath(Common::FTDS::String&& path);

		/// @brief Exposes the scene manager's ordered chunk-path collection.
		/// @return Borrowed access to the chunk list.
		/// @note Changes through the returned reference affect this object's stored state.
		CORE_API Common::FTDS::DynamicArray<Common::FTDS::String*>*& ChunkList();

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param scene Scene managed or processed by the operation.
		CORE_API virtual void Initialize(Scene* scene);
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		CORE_API virtual void ProcessEvent();

	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param ofs Output stream receiving the serialized data.
		CORE_API void SaveSceneList(std::ofstream& ofs);

	private:
		Common::FTDS::String							   mChunkListPath;
		Scene*											   mCurrentScene;
		Common::FTDS::DynamicArray<Common::FTDS::String*>* mChunkList; // List, and order of .chunks
	};

	extern "C"
	{
		/// @brief Searches the scene for an actor matching the supplied identifier or name.
		/// @param name Name used to identify the requested object or interface.
		/// @param filter Actor to exclude from the name search.
		/// @return Borrowed matching entry, or nullptr when the search does not find one.
		CORE_API Common::IActor* FindActor(const char* name, Common::IActor* filter);
	}
} // namespace Core
