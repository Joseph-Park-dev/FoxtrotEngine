// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/ChunkLoader.h"

#include <fstream>

#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
// #include "Manager/CollisionManager.h"
// #include "Manager/SoundManager.h"
#include "FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/Camera.h"
#include "Actor/Transform.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"
#include "FileSystem/DLLPath.h"

#include "Component/Animator.h"
#include "Component/SpriteRenderer.h"
#include "Component/TileMapRenderer.h"
// #include "Component/TextRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/SpineAnimator.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"

#include "Actor/Actor.h"

namespace Editor
{
	using namespace Core;
	ChunkLoader::ChunkLoader()
		: ChunkLoader()
	{
		HMODULE coreMod = GetModuleHandleA(DLLPath::CORE_EDITOR);
		assert(coreMod);

		//// You must list all Components to be used during runtime.
		//// That includes the user defined s, or "Behavior"s.
		// mComponentCreateMap = {
		//	//{ "AI", Create<AI> },
		//	{ "Animator", &CreateComp<D3D11::Animator> },
		//	//{ "BoxCollider2D", &CreateComp<BoxCollider2D> },
		//	//{ "CircleCollider2D", &CreateComp<CircleCollider2D> },
		//	//{ "Rigidbody2D", &CreateComp<Rigidbody2D> },
		//	{ "SpriteRenderer", &CreateComp<D3D11::SpriteRenderer> },
		//	{ "TileMapRenderer", &CreateComp<D3D11::TileMapRenderer> },
		//	//{ "TextRenderer", &CreateComp<D3D11::TextRenderer> },
		//	{ "MeshRenderer", &CreateComp<D3D11::MeshRenderer> },
		//	{ "SpineAnimator", &CreateComp<D3D11::SpineAnimator> },

		//	//// UI related
		//	//{ "ButtonUI", &CreateComp<ButtonUI> },
		//	//{ "PanelUI", &CreateComp<PanelUI> },

		//	//// Actor Behaviors
		//	//{ "Seek", &CreateComp<Seek> },
		//	//{ "Flee", &CreateComp<Flee> },
	};

	ChunkLoader::~ChunkLoader() {}

	void ChunkLoader::SaveChunk(const char* chunkPath)
	{
		Lock();
		std::ofstream ofs(chunkPath);
		SaveActorsData(ofs);

		// Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CORE_RES_DATA);
		//  mGetCoreResManagerFunc()->SaveResourcesToChunk<Core::FTPremade>(ofs);

		ChunkLoader::SaveChunkData(ofs);
		Common::FileIOHelper::SaveBufferToFile(ofs);
		Unlock();
	}

	void ChunkLoader::LoadChunk(const char* chunkPath)
	{
		Lock();
		std::ifstream ifs(chunkPath);
		ChunkLoader::LoadChunkData(ifs);
		// LightManager::GetInstance()->LoadProperties(ifs);
		// CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
		// EditorResourceManager::GetInstance()->PassLoadResourceInChunk(ifs);
		// SoundManager::GetInstance()->LoadProperties(ifs);
		LoadActorsData(ifs);

		// D3D11::Camera::GetInstance()->LoadProperties(ifs);
		Unlock();
	}

	void ChunkLoader::SaveActorsData(std::ofstream& ofs)
	{
		ResetMaxActorID();
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		Common::FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::ACTOR_DATA);

		// Actor's temp ID to be assigned as parent/Children.

		Common::FTDS::DynamicArray<Core::IActor*>* actors = scene->Actors();
		for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
		{
			EditorElement* element = static_cast<EditorElement*>(*actor);
			Common::FileIOHelper::BeginDataPackSave(ofs, element->GetName());
			element->SaveComponents(ofs);
			element->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, element->GetName());
			AddMaxActorID();
		}
		Common::FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::ACTOR_DATA);
	}

	void ChunkLoader::LoadActorsData(std::ifstream& ifs)
	{
		EditorScene*							  scene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::pair<size_t, Common::FTDS::String>&& pack	= Common::FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::ACTOR_DATA);
		std::vector<Actor*>						  actorBuf;
		ResetMaxActorID();

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String>&& actorData = Common::FileIOHelper::BeginDataPackLoad(ifs);
			Actor									  actor		= Actor(Core::ChunkKey::ID::INVALID);
			actor.LoadProperties(ifs);
			actor.LoadComponents(ifs);
			Core::IActor* element = scene->AddEditorElement(&actor);
			element->GetTransform()->SetOwner(element);

			AddMaxActorID();
		}

		Common::FTDS::HashMap<EditorElement*> actorWithIDs;
		actorWithIDs.Reserve(scene->Actors()->GetSize());

		Common::FTDS::DynamicArray<IActor*>* actors = scene->Actors();
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

				if (0 < element->GetChildActors()->GetSize())
				{
					Common::FTDS::DynamicArray<IActor*> children;

					element->GetChildActors()->IterateArray([&](Actor* c) {
						Core::IActor* child = actorWithIDs.At(c->GetID())->Value();
						element->RemoveChild(c);
						delete c;
						c = nullptr;
						children.PushBack(child);
					});
					element->GetChildActors()->Clear();
					element->GetChildActors()->Copy(children);
				}
			}
		}
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

	static void GetDLLPath(HMODULE mod, Common::FTDS::String& out)
	{
		char path[MAX_PATH];
		if (GetModuleFileNameA(mod, path, MAX_PATH))
			out.Assign(path);
		else
			std::cerr << "Failed to get path. Error: " << GetLastError() << std::endl;
	}

	EDITOR_API Editor::ChunkLoader* GetEditorChunkLoader()
	{
		return Editor::ChunkLoader::GetInstance();
	}
} // namespace Editor