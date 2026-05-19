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

namespace Core
{
	void ChunkLoader::SaveChunk(const char* chunkPath)
	{
		::std::ofstream ofs(chunkPath);
		SaveActorsData(ofs);

		SaveChunkData(ofs);
	}

	void ChunkLoader::LoadChunk(const char* chunkPath)
	{
		Lock();
		::std::ifstream ifs(chunkPath);
		LoadChunkData(ifs);

		// Load premades to Core ResourceManager
		size_t premadeCount = Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CORE_RES_DATA).first;
		// Core::ResourceManager::GetInstance()->LoadResourcesFromChunk(ifs, premadeCount);

		LoadActorsData(ifs);
		Unlock();
	}

	void ChunkLoader::Lock()
	{
		mCurrentChunkData->IsLoading = true;
	}

	void ChunkLoader::Unlock()
	{
		mCurrentChunkData->IsLoading = false;
	}

	void ChunkLoader::CopyChunk(const char* chunkPath)
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
	}

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
			Debug::LogError(__LINE__, __FILE__, "Failed to remove copied file: path is empty.");
	}

	const bool ChunkLoader::IsLoadingChunk() const
	{
		return mCurrentChunkData->IsLoading;
	}

	const int ChunkLoader::GetMaxActorID() const
	{
		return mCurrentChunkData->MaxActorID;
	}

	void ChunkLoader::AddMaxActorID()
	{
		++mCurrentChunkData->MaxActorID;
	}

	void ChunkLoader::ResetMaxActorID()
	{
		mCurrentChunkData->MaxActorID = 0;
	}

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

	void ChunkLoader::LoadActorsData(std::ifstream& ifs)
	{
		Scene*									  scene = SceneManager::GetInstance()->GetCurrentScene();
		std::pair<size_t, Common::FTDS::String>&& pack	= Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
		std::vector<Actor*>						  actorBuf;

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String>&& actorData = Common::FileIOHelper::BeginDataPackLoad(ifs);
			Actor* actor										= DBG_NEW Actor(ChunkKey::ID::INVALID);
			actor->LoadProperties(ifs);
			actor->LoadComponents(ifs);
			scene->AddActor(actor);

			AddMaxActorID();
		}

		Common::FTDS::HashMap<Actor*> actorWithIDs;
		actorWithIDs.Reserve(scene->GetActors()->GetSize());

		scene->Actors()->IterateArray([&](Actor* actor) {
			actorWithIDs.Insert(actor->GetID(), actor);
		});

		for (auto iter = scene->Actors()->Begin(); iter != scene->Actors()->End(); ++iter)
		{
			if ((*iter)->GetParent())
			{
				Actor* parent = actorWithIDs.At((*iter)->GetParent()->GetID())->Value();
				delete (*iter)->GetParent();
				(*iter)->SetParent(nullptr);
				(*iter)->SetParent(parent);
			}

			if (0 < (*iter)->GetChildActors()->GetSize())
			{
				Common::FTDS::DynamicArray<Actor*> children;

				(*iter)->GetChildActors()->IterateArray([&](Actor* c) {
					Actor* child = actorWithIDs.At(c->GetID())->Value();
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

	void ChunkLoader::SaveChunkData(std::ofstream& out)
	{
		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		Common::FileIOHelper::BeginDataPackSave(out, ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::SaveInt(out, ChunkKey::ACTOR_COUNT, mCurrentChunkData->MaxActorID);
		Common::FileIOHelper::EndDataPackSave(out, ChunkKey::CHUNK_DATA);
	}

	void ChunkLoader::LoadChunkData(std::ifstream& ifs)
	{
		// Load number of Actors
		int maxActor = 0;
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::LoadInt(ifs, maxActor);

		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->Actors()->Reserve(maxActor);
	}

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

	ChunkLoader::~ChunkLoader()
	{
		DeleteCopiedChunk();
		delete mCurrentChunkData;
	}
} // namespace Core