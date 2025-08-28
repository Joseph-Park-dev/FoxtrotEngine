// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorChunkLoader.h"

#include <fstream>

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/SoundManager.h"
#include "Math/FTMath.h"
#include "Components/BatchHeaders.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/Camera.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorResourceManager.h"

EditorChunkLoader::EditorChunkLoader()
	: ChunkLoader()
{
	// You must list all Components to be used during runtime.
	// That includes the user defined s, or "Behavior"s.
	mComponentCreateMap = {
		{ "AI", &Component::Create<AI> },
		{ "Animator", &Component::Create<Animator> },
		{ "BoxCollider2D", &Component::Create<BoxCollider2D> },
		{ "CircleCollider2D", &Component::Create<CircleCollider2D> },
		{ "InputMove", &Component::Create<InputMove> },
		{ "Move", &Component::Create<Move> },
		{ "Rigidbody2D", &Component::Create<Rigidbody2D> },
		{ "SpriteRenderer", &Component::Create<SpriteRenderer> },
		{ "TileMapRenderer", &Component::Create<TileMapRenderer> },
		{ "TextRenderer", &Component::Create<TextRenderer> },
		{ "MeshRenderer", &Component::Create<MeshRenderer> },
		{ "SpineAnimator", &Component::Create<SpineAnimator> },

		// UI related
		{ "ButtonUI", &Component::Create<ButtonUI> },
		{ "PanelUI", &Component::Create<PanelUI> },

		// Actor Behaviors
		{ "Seek", &Component::Create<Seek> },
		{ "Flee", &Component::Create<Flee> },
	};
}

EditorChunkLoader::~EditorChunkLoader() {}

void EditorChunkLoader::SaveChunk(FTDS::String& fileName)
{
	Lock();
	std::ofstream ofs(fileName.C_Str());
	// Save -> ActorData comes first, // Load -> ChunkData comes first
	Camera::GetInstance()->SaveProperties(ofs);
	SaveActorsData(ofs);
	SoundManager::GetInstance()->SaveProperties(ofs);
	EditorResourceManager::GetInstance()->SaveResources(ofs);
	CollisionManager::GetInstance()->SaveCollisionMarks(ofs);
	// LightManager::GetInstance()->SaveProperties(ofs);
	SaveChunkData(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
	Unlock();
}

void EditorChunkLoader::LoadChunk(FTDS::String& fileName)
{
	Lock();
	std::ifstream ifs(fileName.C_Str());
	LoadChunkData(ifs);
	// LightManager::GetInstance()->LoadProperties(ifs);
	CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
	EditorResourceManager::GetInstance()->PassLoadResourceInChunk(ifs);
	SoundManager::GetInstance()->LoadProperties(ifs);
	LoadActorsData(ifs);

	std::vector<EditorElement*>& elements = EditorSceneManager::GetInstance()->GetEditorScene()->GetEditorElements();
	Camera::GetInstance()->LoadProperties(ifs);
	Unlock();
}

void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
{
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_DATA);

	for (EditorElement* element : scene->GetEditorElements())
	{
		FileIOHelper::BeginDataPackSave(ofs, element->GetName());
		element->SaveComponents(ofs);
		element->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, element->GetName());
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_DATA);
}

void EditorChunkLoader::LoadActorsData(std::ifstream& ifs)
{
	EditorScene*					  scene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::pair<size_t, FTDS::String>&& pack	= FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
	std::vector<Actor*>				  actorBuf;
	for (size_t i = 0; i < pack.first; ++i)
	{
		std::pair<size_t, FTDS::String>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
		Actor							  actor		= Actor();
		actor.LoadProperties(ifs);
		actor.LoadComponents(ifs);
		scene->AddEditorElement(&actor);

		if (actor.GetParent())
		{
			delete actor.GetParent();
			actor.SetParent(nullptr);
		}

		if (0 < actor.GetChildActors().size())
		{
			for (Actor* child : actor.GetChildActors())
			{
				delete child;
				child = nullptr;
			}
		}
	}

	EditorSceneManager::GetInstance()->GetEditorScene()->Initialize(FTCoreEditor::GetInstance());
	EditorSceneManager::GetInstance()->GetEditorScene()->Setup();
}