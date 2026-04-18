// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorChunkLoader.h"

#include <fstream>

#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
//#include "Manager/CollisionManager.h"
//#include "Manager/SoundManager.h"
#include "Math/FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/Camera.h"
#include "Actor/Transform.h"

#include "Component/Animator.h"
#include "Component/SpriteRenderer.h"
#include "Component/TileMapRenderer.h"
//#include "Component/TextRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/SpineAnimator.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorResourceManager.h"
#include "EditorScene.h"

namespace Editor
{
	using namespace Core;
	EditorChunkLoader::EditorChunkLoader()
		: ChunkLoader()
	{
		// You must list all Components to be used during runtime.
		// That includes the user defined s, or "Behavior"s.
		mComponentCreateMap = {
			//{ "AI", Create<AI> },
			{ "Animator", &CreateComp<D3D11::Animator> },
			//{ "BoxCollider2D", &CreateComp<BoxCollider2D> },
			//{ "CircleCollider2D", &CreateComp<CircleCollider2D> },
			//{ "Rigidbody2D", &CreateComp<Rigidbody2D> },
			{ "SpriteRenderer", &CreateComp<D3D11::SpriteRenderer> },
			{ "TileMapRenderer", &CreateComp<D3D11::TileMapRenderer> },
			//{ "TextRenderer", &CreateComp<D3D11::TextRenderer> },
			{ "MeshRenderer", &CreateComp<D3D11::MeshRenderer> },
			{ "SpineAnimator", &CreateComp<D3D11::SpineAnimator> },

			//// UI related
			//{ "ButtonUI", &CreateComp<ButtonUI> },
			//{ "PanelUI", &CreateComp<PanelUI> },

			//// Actor Behaviors
			//{ "Seek", &CreateComp<Seek> },
			//{ "Flee", &CreateComp<Flee> },
		};
	}

	EditorChunkLoader::~EditorChunkLoader() {}

	void EditorChunkLoader::SaveChunk(FTDS::String& fileName)
	{
		Lock();
		std::ofstream ofs(fileName.C_Str());
		// Save -> ActorData comes first, // Load -> ChunkData comes first
		D3D11::Camera::GetInstance()->SaveProperties(ofs);
		SaveActorsData(ofs);
		//SoundManager::GetInstance()->SaveProperties(ofs);
		EditorResourceManager::GetInstance()->SaveResources(ofs);
		//CollisionManager::GetInstance()->SaveCollisionMarks(ofs);
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
		//CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
		EditorResourceManager::GetInstance()->PassLoadResourceInChunk(ifs);
		//SoundManager::GetInstance()->LoadProperties(ifs);
		LoadActorsData(ifs);

		D3D11::Camera::GetInstance()->LoadProperties(ifs, EditorSceneManager::GetInstance());
		Unlock();
	}

	void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
	{
		ResetMaxActorID();
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::ACTOR_DATA);

		// Actor's temp ID to be assigned as parent/Children.

		FTDS::DynamicArray<Actor*>* actors = scene->Actors();
		for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
		{
			EditorElement* element = static_cast<EditorElement*>(*actor);
			FileIOHelper::BeginDataPackSave(ofs, element->GetName());
			element->SaveComponents(ofs);
			element->SaveProperties(ofs);
			FileIOHelper::EndDataPackSave(ofs, element->GetName());
			AddMaxActorID();
		}
		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::ACTOR_DATA);
	}

	void EditorChunkLoader::LoadActorsData(std::ifstream& ifs)
	{
		EditorScene*					  scene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::pair<size_t, FTDS::String>&& pack	= FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::ACTOR_DATA);
		std::vector<Actor*>				  actorBuf;
		ResetMaxActorID();

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, FTDS::String>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
			Actor							  actor		= Actor(Core::ChunkKey::ID::INVALID);
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
} // namespace Editor