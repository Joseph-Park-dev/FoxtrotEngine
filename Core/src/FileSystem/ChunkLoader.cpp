// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/ChunkLoader.h"

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Math/FTMath.h"
#include "TemplateFunctions.h"
#include "Manager/SceneManager.h"
#include "Manager/ResourceManager.h"
#include "Scene/Scene.h"
#include "Actor/ActorGroup.h"
#include "Actor/Transform.h"
#include "Actor/Actor.h"
#include "FileSystem/FileIOHelper.h"
#include "Static/FTString.h"
#include "Static/HashMap.h"
#include "Plugin/IPlugin.h"
#include "ResourceSystem/FTPremade.h"
#include "Engine.h"

namespace Core
{
	void ChunkLoader::SaveChunk(FTDS::String& fileName)
	{
		std::ofstream ofs(fileName.C_Str());
		SaveActorsData(ofs);

		SaveChunkData(ofs);
	}

	void ChunkLoader::LoadChunk(FTDS::String& fileName)
	{
		Lock();
		std::ifstream ifs(fileName.C_Str());
		LoadChunkData(ifs);
		LoadPlugins(ifs);

		// Load premades to Core ResourceManager
		size_t premadeCount = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CORE_RES_DATA).first;
		Core::ResourceManager::GetInstance()->LoadResourcesFromChunk<FTPremade>(ifs, premadeCount);

		LoadActorsData(ifs);
		Unlock();
	}

	void ChunkLoader::Lock()
	{
		mIsLoading = true;
	}

	void ChunkLoader::Unlock()
	{
		mIsLoading = false;
	}

	void ChunkLoader::CopyChunk(FTDS::String& path)
	{
		// Get the original file name.
		std::filesystem::path original = path.C_Str();

		// Get the copied file name.
		FTDS::String copiedPath;
		path.ExtractUntilLast(copiedPath, "\\");
		copiedPath.Append("\\");

		// Get the full copied file path.
		FTDS::String copiedName = ExtractFileName(path.C_Str());
		copiedName.ExtractUntilFirst(copiedName, ".");
		copiedName.Append(" Copy.chunk");
		copiedPath.Append(copiedName);

		std::filesystem::path copied = copiedPath.C_Str();

		// Copy the selected .chunk file to load into the game.
		std::filesystem::copy_file(
			original,
			copied,
			std::filesystem::copy_options::overwrite_existing);

		// Assign the copied file name as current.
		mCurrentChunkCopy.Assign(copiedPath);
	}

	void ChunkLoader::DeleteCopiedChunk()
	{
		if (!mCurrentChunkCopy.IsEmpty())
		{
			std::filesystem::remove(mCurrentChunkCopy.C_Str());
			mCurrentChunkCopy.Clear();
		}
	}

	const bool ChunkLoader::IsLoadingChunk() const
	{
		return mIsLoading;
	}

	FTDS::String& ChunkLoader::CurrentChunk()
	{
		return mCurrentChunkCopy;
	}

	const int ChunkLoader::GetMaxActorID() const
	{
		return mMaxActorID;
	}

	FTDS::HashMap<FARPROC>* ChunkLoader::GetCompConstructors()
	{
		return mCompConstructors;
	}

	void ChunkLoader::AddMaxActorID()
	{
		++mMaxActorID;
	}

	void ChunkLoader::ResetMaxActorID()
	{
		mMaxActorID = 0;
	}

	void ChunkLoader::SaveActorsData(std::ofstream& out)
	{
	}

	void ChunkLoader::LoadActorsData(std::ifstream& ifs)
	{
		Scene*							  scene = SceneManager::GetInstance()->GetCurrentScene();
		std::pair<size_t, FTDS::String>&& pack	= FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
		std::vector<Actor*>				  actorBuf;

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, FTDS::String>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
			Actor* actor								= DBG_NEW Actor(ChunkKey::ID::INVALID);
			actor->LoadProperties(ifs);
			actor->LoadComponents(ifs);
			scene->AddActor(actor);

			AddMaxActorID();
		}

		FTDS::HashMap<Actor*> actorWithIDs;
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
				FTDS::DynamicArray<Actor*> children;

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

	void ChunkLoader::LoadPlugins(std::ifstream& ifs)
	{
		size_t dllCount = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Plugin::PLUGIN_DATA).first;
		for (size_t i = 0; i < dllCount; ++i)
		{
			FTDS::String dllPath	= {};
			FTDS::String pluginName = {};
			ExtractFileName(dllPath, pluginName);

			HMODULE mod = LoadLibraryA(dllPath.C_Str());
			Engine::GetInstance()->RegisterPlugin(mod, pluginName.C_Str());
			LoadCompConstructors(ifs, mod);
			LoadManagerData(ifs, mod);
		}
	}

	void ChunkLoader::LoadCompConstructors(std::ifstream& ifs, HMODULE& mod)
	{
		size_t count = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Plugin::COMP_CONSTRUCTORS).first;
		mCompConstructors->Reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			FTDS::String compName	  = {};
			FTDS::String compProcName = {};
			FileIOHelper::LoadBasicString(ifs, compName);
			compProcName.Assign(compName);
			compProcName.Append("_Create");

			mCompConstructors->Insert(compProcName, GetProcAddress(mod, compName.C_Str()));
		}
	}

	void ChunkLoader::LoadManagerData(std::ifstream& ifs, HMODULE& mod)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Plugin::MANAGER_DATA);
		// plugin->LoadManagerData(ifs);
	}

	void ChunkLoader::SaveChunkData(std::ofstream& out)
	{
		Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
		FileIOHelper::BeginDataPackSave(out, ChunkKey::CHUNK_DATA);
		FileIOHelper::SaveInt(out, ChunkKey::ACTOR_COUNT, mMaxActorID);
		FileIOHelper::EndDataPackSave(out, ChunkKey::CHUNK_DATA);
	}

	void ChunkLoader::LoadChunkData(std::ifstream& ifs)
	{
		// Load number of Actors
		int maxActor = 0;
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHUNK_DATA);
		FileIOHelper::LoadInt(ifs, maxActor);

		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->Actors()->Reserve(maxActor);
	}

	ChunkLoader::ChunkLoader()
		: mCurrentChunkData{}
		, mIsLoading(false)
		, mCurrentChunkCopy()
		, mMaxActorID(ChunkKey::ID::CLONE) {
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
	}
} // namespace Core