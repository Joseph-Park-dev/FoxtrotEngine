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

#include "Utils/UUIDGenerator.h"

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

	Camera::GetInstance()->LoadProperties(ifs);
	Unlock();
}

void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
{
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::ACTOR_DATA);

	// Actor's temp ID to be assigned as parent/Children.

	FTDS::DynamicArray<Actor*>* actors = scene->Actors();
	for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
	{
		EditorElement* element = static_cast<EditorElement*>(*actor);
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
		Actor							  actor		= Actor(ChunkKey::ID::INVALID);
		actor.LoadProperties(ifs);
		actor.LoadComponents(ifs);
		EditorElement* element = scene->AddEditorElement(&actor);
		element->GetTransform()->SetOwner(element);

		AddMaxActorID();
	}

	FTDS::HashMap<EditorElement*> actorWithIDs;
	actorWithIDs.Reserve(scene->Actors()->GetSize());

	FTDS::DynamicArray<Actor*>* actors = scene->Actors();
	for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
	{
		if (*actor)
		{
			EditorElement* element = static_cast<EditorElement*>(*actor);
			actorWithIDs.Insert(element->GetID(), element);
		}
	}

	for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
	{
		EditorElement* element = static_cast<EditorElement*>(*actor);
		if (element)
		{
			if (element->GetParent())
			{
				EditorElement* parent = actorWithIDs.At(element->GetParent()->GetID())->Value();
				delete element->GetParent();
				element->SetParent(nullptr);
				element->SetParent(parent);
			}

			if (0 < element->GetChildActors().GetSize())
			{
				FTDS::DynamicArray<Actor*> children;

				element->GetChildActors().IterateArray([&](Actor* c) {
					Actor* child = actorWithIDs.At(c->GetID())->Value();
					element->RemoveChild(c);
					delete c;
					c = nullptr;
					children.PushBack(child);
				});
				element->GetChildActors().Clear();
				element->GetChildActors().Copy(children);
			}
		}
	}

	scene->Initialize(FTCoreEditor::GetInstance());
	scene->Setup();
}