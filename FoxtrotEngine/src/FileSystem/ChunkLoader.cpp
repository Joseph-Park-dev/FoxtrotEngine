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
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <filesystem>

#include "Math/FTMath.h"
#include "Core/TemplateFunctions.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/SoundManager.h"
#include "Scenes/Scene.h"
#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Renderer/Camera.h"

#include "FileSystem/FileIOHelper.h"
#include "Components/BatchHeaders.h"

void ChunkLoader::SaveChunk(FTDS::String& fileName)
{
	std::ofstream ofs(fileName.C_Str());
	SaveChunkData(ofs);
}

void ChunkLoader::LoadChunk(FTDS::String& fileName)
{
	Lock();
	std::ifstream ifs(fileName.C_Str());
	LoadChunkData(ifs);
	// LightManager::GetInstance()->LoadProperties(ifs);
	CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
	ResourceManager::GetInstance()->LoadResources(ifs);
	SoundManager::GetInstance()->LoadProperties(ifs);
	LoadActorsData(ifs);

	Camera::GetInstance()->LoadProperties(ifs);
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
		std::filesystem::copy_options::overwrite_existing
	);

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

void ChunkLoader::AddMaxActorID()
{
	++mMaxActorID;
}

void ChunkLoader::SaveChunkData(std::ofstream& out)
{
	Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
	FileIOHelper::BeginDataPackSave(out, ChunkKey::CHUNK_DATA);
	FileIOHelper::SaveInt(out, ChunkKey::ACTOR_COUNT, mMaxActorID);
	FileIOHelper::EndDataPackSave(out, ChunkKey::CHUNK_DATA);
}

void ChunkLoader::SaveActorsData(std::ofstream& out)
{
}

void ChunkLoader::LoadActorsData(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
	for (size_t i = 0; i < pack.first; ++i)
	{
		std::pair<size_t, FTDS::String>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);

		Actor* actor = DBG_NEW Actor(ChunkKey::ID::INVALID);
		actor->LoadProperties(ifs);
		actor->LoadComponents(ifs);
		actor->Initialize(FTCore::GetInstance());
		actor->Setup();

		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->AddActor(actor, actor->GetActorGroup());
	}
}

void ChunkLoader::LoadChunkData(std::ifstream& ifs)
{
	int maxActor = 0;
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHUNK_DATA);
	FileIOHelper::LoadInt(ifs, maxActor);
}

ChunkLoader::ChunkLoader()
	: mCurrentChunkData{}
	, mIsLoading(false)
	, mCurrentChunkCopy()
	, mMaxActorID(ChunkKey::ID::CLONE)
{
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

	mComponentLoadMap.Reserve(25);
	mComponentLoadMap.Insert("AI", &Component::Load<AI>);
	mComponentLoadMap.Insert("Animator", &Component::Load<Animator>);
	mComponentLoadMap.Insert("BoxCollider2D", &Component::Load<BoxCollider2D>);
	mComponentLoadMap.Insert("CircleCollider2D", &Component::Load<CircleCollider2D>);
	mComponentLoadMap.Insert("Rigidbody2D", &Component::Load<Rigidbody2D>);
	mComponentLoadMap.Insert("SpriteRenderer", &Component::Load<SpriteRenderer>);
	mComponentLoadMap.Insert("TileMap", &Component::Load<TileMapRenderer>);
	mComponentLoadMap.Insert("TextRenderer", &Component::Load<TextRenderer>);
	mComponentLoadMap.Insert("MeshRenderer", &Component::Load<MeshRenderer>);
	mComponentLoadMap.Insert("SpineAnimator", &Component::Load<SpineAnimator>);
	mComponentLoadMap.Insert("ButtonUI", &Component::Load<ButtonUI>);
	mComponentLoadMap.Insert("Seek", &Component::Load<Seek>);
	mComponentLoadMap.Insert("Flee", &Component::Load<Flee>);
};

ChunkLoader::~ChunkLoader()
{
	DeleteCopiedChunk();
}