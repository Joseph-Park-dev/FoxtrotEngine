// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/ChunkLoader.h"

#include <fstream>
#include <filesystem>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"
#include "Manager/SceneManager.h"
#include "Scene/Scene.h"
#include "FTDS/Static/HashMap.h"
#include "Actor/Actor.h"
#include "Manager/PluginManager.h"
#include "FTDS/Static/FTString.h"

namespace Core
{
	/// @brief Serializes the current scene and resources to the selected .chunk file.
	/// @param chunkPath Path to the .chunk file.
	void ChunkLoader::SaveChunk(const char* chunkPath)
	{
		::std::ofstream ofs(chunkPath);
		SaveActorsData(ofs);

		SaveChunkData(ofs);
	}

	/// @brief Restores the scene and its resources from a .chunk file.
	/// @param chunkPath Path to the .chunk file.
	void ChunkLoader::LoadChunk(const char* chunkPath)
	{
		Lock();
		::std::ifstream ifs(chunkPath);
		LoadChunkData(ifs);
		LoadActorProperties(ifs);
		// LoadPlugins(ifs);

		// Plugin Data;
		//		-Manager  Data;
		//		-Resource Data;

		// Actor Data;
		//		-Loads Actor Properties;
		//		-Register	 Components;

		// Load premades to Core ResourceManager
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RES_DATA).first;

		// Core::ResourceManager::GetInstance()->LoadResourcesFromChunk(ifs, premadeCount);

		Unlock();
	}

	/// @brief Pauses game updates while chunk state is being changed.
	void ChunkLoader::Lock()
	{
		mCurrentChunkData->IsLoading = true;
	}

	/// @brief Resumes game updates after chunk processing completes.
	void ChunkLoader::Unlock()
	{
		mCurrentChunkData->IsLoading = false;
	}

	/// @brief Creates a temporary copy of a chunk so the source remains available during loading.
	/// @param copiedPathOut Receives the path of the temporary chunk copy.
	/// @param chunkPath Path to the .chunk file.
	void ChunkLoader::CopyChunk(Common::FTDS::String& copiedPathOut, const char* chunkPath)
	{
		// Get the original file name.
		Common::FTDS::String path = chunkPath;

		// Get the copied file name.
		Common::FTDS::String copiedPath;
		path.ExtractUntilLast(copiedPath, "\\");
		copiedPath.Append("\\");

		// Get the full copied file path.
		Common::FTDS::String copiedName = Common::ExtractFileName(path.C_Str());
		copiedName.ExtractUntilFirst(copiedName, ".");
		copiedName.Append(" Copy.chunk");
		copiedPath.Append(copiedName);

		std::filesystem::path copied = copiedPath.C_Str();

		// Copy the selected .chunk file to load into the game.
		std::filesystem::copy_file(
			std::filesystem::path(path.C_Str()),
			copied,
			std::filesystem::copy_options::overwrite_existing);

		// Assign the copied file name as current.
		mCurrentChunkData->Path.Assign(path);
		copiedPathOut.Assign(copiedPath);
	}

	/// @brief Removes the temporary chunk copy created for loading.
	void ChunkLoader::DeleteCopiedChunk()
	{
		// Get the copied file name.
		Common::FTDS::String copiedPath;
		mCurrentChunkData->Path.ExtractUntilLast(copiedPath, "\\");
		copiedPath.Append("\\");

		// Get the full copied file path.
		Common::FTDS::String copiedName = Common::ExtractFileName(mCurrentChunkData->Path.C_Str());
		copiedName.ExtractUntilFirst(copiedName, ".");
		copiedName.Append(" Copy.chunk");
		copiedPath.Append(copiedName);

		if (!copiedPath.IsEmpty())
			std::filesystem::remove(copiedPath.C_Str());
		else
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to remove copied file: path is empty.");
	}

	/// @brief Tests the loading chunk condition for the current object.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	const bool ChunkLoader::IsLoadingChunk() const
	{
		return mCurrentChunkData->IsLoading;
	}

	/// @brief Returns the max actor id used by this chunk loader.
	/// @return Current max actor id.
	const int ChunkLoader::GetMaxActorID() const
	{
		return mCurrentChunkData->MaxActorID;
	}

	/// @brief Advances the actor identifier counter used during chunk loading.
	void ChunkLoader::AddMaxActorID()
	{
		++mCurrentChunkData->MaxActorID;
	}

	/// @brief Resets the actor identifier counter before rebuilding scene content.
	void ChunkLoader::ResetMaxActorID()
	{
		mCurrentChunkData->MaxActorID = 0;
	}

	/// @brief Writes actor properties and components for the current scene.
	/// @param ofs Output stream receiving the serialized data.
	void ChunkLoader::SaveActorsData(std::ofstream& ofs)
	{
		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_DATA);
		for (auto iter = scene->Actors()->Begin(); iter != scene->Actors()->End(); ++iter)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, (*iter)->GetNameRef());
			(*iter)->SaveComponents(ofs);
			(*iter)->SaveProperties(ofs);
		}
	}

	/// @brief Creates or restores actors from the property sections of the chunk.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void ChunkLoader::LoadActorProperties(std::ifstream& ifs)
	{
		Scene*									  scene = SceneManager::GetInstance()->GetCurrentScene();
		std::pair<size_t, Common::FTDS::String>&& pack	= Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
		std::vector<Actor*>						  actorBuf;

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String>&& actorData = Common::FileIOHelper::BeginDataPackLoad(ifs);
			Actor* actor										= DBG_NEW Actor(Common::ChunkKey::ID::INVALID);
			actor->LoadProperties(ifs);
			scene->AddActor(actor);

			AddMaxActorID();
		}

		Common::FTDS::HashMap<Common::IActor*> actorWithIDs;
		actorWithIDs.Reserve(scene->GetActors()->GetSize());

		scene->Actors()->IterateArray([&](Common::IActor* actor) {
			actorWithIDs.Insert(actor->GetID(), actor);
		});

		for (auto iter = scene->Actors()->Begin(); iter != scene->Actors()->End(); ++iter)
		{
			if ((*iter)->GetParent())
			{
				Common::IActor* parent = actorWithIDs.At((*iter)->GetParent()->GetID())->Value();
				delete (*iter)->GetParent();
				(*iter)->SetParent(nullptr);
				(*iter)->SetParent(parent);
			}

			if (0 < (*iter)->GetChildActors()->GetSize())
			{
				Common::FTDS::DynamicArray<Common::IActor*> children;

				(*iter)->GetChildActors()->IterateArray([&](Common::IActor* c) {
					Common::IActor* child = actorWithIDs.At(c->GetID())->Value();
					(*iter)->RemoveChild(c);
					delete c;
					c = nullptr;
					children.PushBack(child);
				});
				(*iter)->GetChildActors()->Clear();
				(*iter)->GetChildActors()->Copy(children);
			}
		}
	}

	/// @brief Restores component sections after actor properties have been loaded.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void ChunkLoader::LoadActorComponents(::std::ifstream& ifs)
	{
		std::pair<size_t, Common::FTDS::String>&& pack	= Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_COMP);
		Scene*									  scene = SceneManager::GetInstance()->GetCurrentScene();
		for (auto iter = scene->Actors()->Begin(); iter != scene->Actors()->End(); ++iter)
		{
			Common::FileIOHelper::BeginDataPackLoad(ifs, (*iter)->GetName());
			(*iter)->LoadComponents(ifs);
		}
	}

	// void ChunkLoader::LoadPlugins(std::ifstream& ifs)
	//{
	//	size_t count = Common::FileIOHelper::BeginDataPackLoad(ifs, Common::ChunkKey::Plugin::PLUGIN_DATA).first;

	//	for (size_t i = 0; i < count; ++i)
	//	{
	//		Common::FTDS::String name;
	//		Common::FileIOHelper::LoadBasicString(ifs, name);

	//		Common::IPlugin* plg = Core::PluginManager::GetInstance()->RegisterPlugin(name.C_Str());
	//		plg->LoadProperties(ifs);
	//	}
	//}

	/// @brief Writes the chunk-specific scene metadata to the output stream.
	/// @param out Receives the operation's output.
	void ChunkLoader::SaveChunkData(std::ofstream& out)
	{
		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		Common::FileIOHelper::BeginDataPackSave(out, ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::SaveInt(out, ChunkKey::ACTOR_COUNT, mCurrentChunkData->MaxActorID);
		Common::FileIOHelper::EndDataPackSave(out, ChunkKey::CHUNK_DATA);
	}

	/// @brief Reads chunk-specific scene metadata from the input stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void ChunkLoader::LoadChunkData(std::ifstream& ifs)
	{
		// Load number of Actors
		int maxActor = 0;
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::LoadInt(ifs, maxActor);

		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->Actors()->Reserve(maxActor);
	}

	/// @brief Initializes chunk-loading state and actor identifier tracking.
	/// @note Initializes the :ChunkLoader base or delegates to its constructor.
	ChunkLoader::ChunkLoader()
		: mCurrentChunkData(DBG_NEW ChunkData) {
			/*mComponentLoadMap = {
				{ "AI", &Component::Load<AI> },
				{ "Animator", &Component::Load<Animator> },
				{ "BoxCollider2D", &Component::Load<BoxCollider2D> },
				{ "InputMove", &Component::Load<InputMove> },
				{ "Move", &Component::Load<Move> },
				{ "Rigidbody2D", &Component::Load<Rigidbody2D> },
				{ "SpriteRenderer", &Component::Load<SpriteRenderer> },
				{ "TileMap", &Component::Load<TileMapRenderer> },
				{ "TextRenderer", &Component::Load<TextRenderer> },
				{ "MeshRenderer", &Component::Load<MeshRenderer> },
				{ "SpineAnimator", &Component::Load<SpineAnimator> },

				{ "ButtonUI", &Component::Load<ButtonUI> },

				{ "Seek", &Component::Load<Seek> },
				{ "Flee", &Component::Load<Flee> },
			};*/

			// mComponentLoadMap.Reserve(25);
			// mComponentLoadMap.Insert("AI", &Component::Load<AI>);
			// mComponentLoadMap.Insert("Animator", &Component::Load<Animator>);
			// mComponentLoadMap.Insert("BoxCollider2D", &Component::Load<BoxCollider2D>);
			// mComponentLoadMap.Insert("CircleCollider2D", &Component::Load<CircleCollider2D>);
			// mComponentLoadMap.Insert("Rigidbody2D", &Component::Load<Rigidbody2D>);
			// mComponentLoadMap.Insert("SpriteRenderer", &Component::Load<SpriteRenderer>);
			// mComponentLoadMap.Insert("TileMap", &Component::Load<TileMapRenderer>);
			// mComponentLoadMap.Insert("TextRenderer", &Component::Load<TextRenderer>);
			// mComponentLoadMap.Insert("MeshRenderer", &Component::Load<MeshRenderer>);
			// mComponentLoadMap.Insert("SpineAnimator", &Component::Load<SpineAnimator>);
			// mComponentLoadMap.Insert("ButtonUI", &Component::Load<ButtonUI>);
			// mComponentLoadMap.Insert("Seek", &Component::Load<Seek>);
			// mComponentLoadMap.Insert("Flee", &Component::Load<Flee>);
		};

	/// @brief Releases the resources managed by this instance during destruction.
	ChunkLoader::~ChunkLoader()
	{
		DeleteCopiedChunk();
		delete mCurrentChunkData;
	}
} // namespace Core
