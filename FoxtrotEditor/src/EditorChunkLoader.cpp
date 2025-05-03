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

void EditorChunkLoader::SaveChunk(const std::string fileName)
{
	Lock();
	std::ofstream ofs(fileName);
	// Save -> ActorData comes first, // Load -> ChunkData comes first
	Camera::GetInstance()->SaveProperties(ofs);
	SaveActorsData(ofs);
	EditorResourceManager::GetInstance()->SaveResources(ofs);
	CollisionManager::GetInstance()->SaveCollisionMarks(ofs);
	LightManager::GetInstance()->SaveProperties(ofs);
	SaveChunkData(ofs);
	//FileIOHelper::SaveBufferToFile(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
	Unlock();
}

void EditorChunkLoader::LoadChunk(const std::string fileName)
{
	Lock();
	std::ifstream ifs(fileName);
	LoadChunkData(ifs);
	LightManager::GetInstance()->LoadProperties(ifs);
	CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
	EditorResourceManager::GetInstance()->LoadResources(ifs, FTCoreEditor::GetInstance());
	LoadActorsData(ifs);
	Camera::GetInstance()->LoadProperties(ifs);
	Unlock();
}

void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
{
	EditorScene*		 scene	= EditorSceneManager::GetInstance()->GetEditorScene();
	std::vector<Actor*>* actors = scene->GetActors();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_DATA);

	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < actors[i].size(); ++j)
		{
			EditorElement* element = dynamic_cast<EditorElement*>(actors[i][j]);
			size_t		   index   = (size_t)ActorGroup::END * i + j;
			FileIOHelper::BeginDataPackSave(ofs, element->GetName());
			element->SaveComponents(ofs);
			element->SaveProperties(ofs);
			FileIOHelper::EndDataPackSave(ofs, element->GetName());
		}
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::ACTOR_DATA);
}

void EditorChunkLoader::LoadActorsData(std::ifstream& ifs)
{
	EditorScene*					 scene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::pair<size_t, std::string>&& pack  = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::ACTOR_DATA);
	for (size_t i = 0; i < pack.first; ++i)
	{
		std::pair<size_t, std::string>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
		Actor							 actor	   = Actor();
		actor.LoadProperties(ifs);
		actor.LoadComponents(ifs);

		EditorElement* element = DBG_NEW EditorElement(&actor, scene);
		element->Initialize(FTCoreEditor::GetInstance());
	}
	EditorSceneManager::GetInstance()->GetEditorScene()->Setup();
}