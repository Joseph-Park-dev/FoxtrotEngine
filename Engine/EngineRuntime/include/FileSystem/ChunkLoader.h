// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This loads the currently designated .Chunk file for the Scene.
/// A .Chunk file is consist of...
/// 1. Chunk data   : defines the data for the .Chunk itself.
/// 2. Manager data : CollisionMarks from CollisionManager,
///                   FTResources loaded by ResourceManager, etc.
/// 3. Actor data   : Data for the Actors. ActorProperties and
///                   Components assigned to each Actor.
/// 4. Camera Property : TargetActor, Offset, etc.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "FileSystem/IChunkLoader.h"

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"

namespace Core
{
	class CORE_API ChunkLoader :
		public IChunkLoader
	{
		SINGLETON(ChunkLoader)

		// Member Functions for editor level to generate chunk.json files
	public:
		/// @brief Serializes the current scene and resources to the selected .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void SaveChunk(const char* chunkPath) override;
		/// @brief Restores the scene and its resources from a .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void LoadChunk(const char* chunkPath) override;

		/// @brief Pauses game updates while chunk state is being changed.
		virtual void Lock() override;
		/// @brief Resumes game updates after chunk processing completes.
		virtual void Unlock() override;

		/// @brief Creates a temporary copy of a chunk so the source remains available during loading.
		/// @param copiedPathOut Receives the path of the temporary chunk copy.
		/// @param chunkPath Path to the .chunk file.
		virtual void CopyChunk(Common::FTDS::String& copiedPathOut, const char* chunkPath = "./") override;
		/// @brief Removes the temporary chunk copy created for loading.
		virtual void DeleteCopiedChunk() override;

	public:
		// ComponentLoadMap& GetComponentLoadMap() { return mComponentLoadMap; }

		/// @brief Tests the loading chunk condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		virtual const bool IsLoadingChunk() const override;
		/// @brief Returns the max actor id used by this chunk loader.
		/// @return Current max actor id.
		virtual const int  GetMaxActorID() const override;

		// Add actor count by 1.
		/// @brief Advances the actor identifier counter used during chunk loading.
		virtual void AddMaxActorID() override;
		/// @brief Resets the actor identifier counter before rebuilding scene content.
		virtual void ResetMaxActorID() override;

	protected:
		/// @brief Writes the chunk-specific scene metadata to the output stream.
		/// @param out Receives the operation's output.
		virtual void SaveChunkData(::std::ofstream& out) override;
		/// @brief Reads chunk-specific scene metadata from the input stream.
		/// @param out Receives the operation's output.
		virtual void LoadChunkData(::std::ifstream& out) override;

	protected:
		/// @brief Writes actor properties and components for the current scene.
		/// @param ofs Output stream receiving the serialized data.
		void SaveActorsData(::std::ofstream& ofs);
		/// @brief Creates or restores actors from the property sections of the chunk.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		void LoadActorProperties(::std::ifstream& ifs);
		/// @brief Restores component sections after actor properties have been loaded.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		void LoadActorComponents(::std::ifstream& ifs);

	private:
		ChunkData* mCurrentChunkData;

	private:
		// void LoadPlugins(std::ifstream& ifs);
	};
} // namespace Core
