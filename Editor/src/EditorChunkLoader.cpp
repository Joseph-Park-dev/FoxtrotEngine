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
// #include "Manager/CollisionManager.h"
// #include "Manager/SoundManager.h"
#include "Math/FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/Camera.h"
#include "Actor/Transform.h"
#include "Plugin/Plugin.h"
#include "Plugin/PluginKey.h"

#include "Component/Animator.h"
#include "Component/SpriteRenderer.h"
#include "Component/TileMapRenderer.h"
// #include "Component/TextRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/SpineAnimator.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"

namespace Editor
{
	using namespace Core;
	EditorChunkLoader::EditorChunkLoader()
		: ChunkLoader()
		, mGetCoreResManagerFunc(nullptr)
	{
		HMODULE coreMod = GetModuleHandleW(Core::DLL);
		assert(coreMod);
		mGetCoreResManagerFunc = reinterpret_cast<Core::GET_RES_MANAGER_INST>(
			GetProcAddress(coreMod, Core::PluginKey::GET_RES_MANAGER));

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
		SaveActorsData(ofs);

		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CORE_RES_DATA);
		mGetCoreResManagerFunc()->SaveResourcesToChunk<Core::FTPremade>(ofs);

		SavePlugins(ofs);
		ChunkLoader::SaveChunkData(ofs);
		FileIOHelper::SaveBufferToFile(ofs);
		Unlock();
	}

	void EditorChunkLoader::LoadChunk(FTDS::String& fileName)
	{
		Lock();
		std::ifstream ifs(fileName.C_Str());
		ChunkLoader::LoadChunkData(ifs);
		// LightManager::GetInstance()->LoadProperties(ifs);
		// CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
		EditorResourceManager::GetInstance()->PassLoadResourceInChunk(ifs);
		// SoundManager::GetInstance()->LoadProperties(ifs);
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
	}
	void EditorChunkLoader::SavePlugins(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Plugin::PLUGIN_DATA);
		FTDS::HashMap<Plugin*>* plugins = GetBase()->GetPlugins();

		size_t dllIdx = 0;
		for (auto iter = plugins->Begin(); iter != plugins->End(); ++iter)
		{
			Plugin*		 plugin	 = (*iter)->Value();
			FTDS::String dllPath = {};
			GetDLLPath(plugin->GetModule(), dllPath);

			FileIOHelper::BeginDataPackSave(ofs, (*iter)->Key());
			FileIOHelper::SaveString(ofs, ChunkKey::Plugin::DLL_PATH, dllPath);
			SaveCompConstructors(ofs, (*iter)->Value());
			SaveManagerData(ofs, (*iter)->Value());
		}
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::Plugin::PLUGIN_DATA);
	}

	void EditorChunkLoader::SaveCompConstructors(std::ofstream& ofs, Plugin* plugin)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Plugin::COMP_CONSTRUCTORS);
		FTDS::HashMap<FARPROC>* map = GetCompConstructors();

		size_t compIdx = 0;
		for (auto iter = map->Begin(); iter != map->End(); ++iter)
			FileIOHelper::SaveString(ofs, std::to_string(compIdx).c_str(), (*iter)->Key().C_Str());

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::Plugin::COMP_CONSTRUCTORS);
	}

	void EditorChunkLoader::SaveManagerData(std::ofstream& ofs, Plugin* plugin)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Plugin::MANAGER_DATA);
		plugin->SaveManagerData(ofs);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::Plugin::MANAGER_DATA);
	}

	static void GetDLLPath(HMODULE mod, FTDS::String& out)
	{
		char path[MAX_PATH];
		if (GetModuleFileNameA(mod, path, MAX_PATH))
			out.Assign(path);
		else
			std::cerr << "Failed to get path. Error: " << GetLastError() << std::endl;
	}
} // namespace Editor