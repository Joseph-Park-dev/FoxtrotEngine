// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton that loads .Chunk to the Foxtrot Editor.
/// .Chunk file, which has a similar purpose with .scene file in UnityEngine,
/// contains all information to organize a scene with various Actors, resources, etc.
///
/// This object contains a map that has ComponentCreateFuncs which will create & assign
/// a new Component to an Actor. You must list all Components to be used into this map.
/// </summary>

#pragma once
#include "FileSystem/IChunkLoader.h"

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"

namespace Editor
{
	//// This creates a Component, initializes and assigns it to an Actor.
	// using ComponentCreateFunc = std::function<void(Core::Actor*, Core::FTCore*)>;

	//// You must list all Components to be used during runtime to this map instance.
	// using ComponentCreateMap = std::unordered_map<const char*, ComponentCreateFunc>;

	// template <class T>
	// static void CreateComp(Core::Plugin* plugin, Core::Actor* actor, int updateOrder)
	//{
	//	// Dynamically allocate actor of type T
	//	T* t = Create(plugin, actor, updateOrder);
	//	// Call LoadProperties on DBG_NEW actor
	//	t->Initialize();
	// }

	class ChunkLoader :
		public Core::IChunkLoader
	{
		SINGLETON(ChunkLoader)
	public:
		// Saves the current scene on Foxtrot Editor into .Chunk file.
		/// @brief Serializes the current scene and resources to the selected .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void SaveChunk(const char* chunkPath) override;
		// Loads the a .Chunk file into the current scene on Foxtrot Editor.
		/// @brief Restores the scene and its resources from a .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void LoadChunk(const char* chunkPath) override;

		/// @brief Halts gameloop while loading a .chunk
		virtual void Lock() override;

		/// @brief Resume gameloop after finishing .chunk loading.
		virtual void Unlock() override;

		/// @brief To prevent .chunk from corrupting, the copy of the file is made before loading.
		/// The copied .chunk is the one that should be read into the game.
		/// @param path The copy is recommended to be located in the same directory with the original.
		/// @return Full path of the copied .chunk
		/// @param copiedPathOut Receives the path of the temporary chunk copy.
		/// @param chunkPath Path to the .chunk file.
		virtual void CopyChunk(Common::FTDS::String& copiedPathOut, const char* chunkPath = "./") override;
		/// @brief Delete the copied chunk after being used.
		virtual void DeleteCopiedChunk() override;

	public:
		/// @brief Tests the loading chunk condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		virtual const bool IsLoadingChunk() const override;
		/// @brief Returns the max actor id used by this chunk loader.
		/// @return Current max actor id.
		virtual const int  GetMaxActorID() const override;
		/// @brief Returns the chunk data used by this chunk loader.
		/// @return Borrowed access to the chunk data.
		Core::ChunkData*   GetChunkData();

		// Add actor count by 1.
		/// @brief Advances the actor identifier counter used during chunk loading.
		virtual void AddMaxActorID() override;
		/// @brief Resets the actor identifier counter before rebuilding scene content.
		virtual void ResetMaxActorID() override;

	protected:
		/// @brief Save .chunk specific data
		/// @param out Receives the operation's output.
		virtual void SaveChunkData(::std::ofstream& out) override;
		/// @brief Load .chunk specific data
		/// @param out Receives the operation's output.
		virtual void LoadChunkData(::std::ifstream& out) override;
		// Takes the values from EditorElements to save them into .Chunk file.
		/// @brief Writes actor properties and components for the current scene.
		/// @param ofs Output stream receiving the serialized data.
		virtual void SaveActorsData(std::ofstream& ofs);
		// Reads the values from .Chunk file and assign them to Actors.
		/// @brief Restores serialized actor state for the loaded scene.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		virtual void LoadActorsData(std::ifstream& ifs);

	private:
		Core::ChunkData* mCurrentChunkData;
	};

#include "Plugin/EditorExports.h"

	constexpr const char* GET_EDITOR_CHUNK_LOADER_FUNC = "GetEditorChunkLoader";
	/// @brief Returns the editor chunk loader used by this service.
	/// @return Borrowed access to the editor chunk loader.
	extern "C" EDITOR_API Editor::ChunkLoader* GetEditorChunkLoader();
} // namespace Editor
