#include "FileSystem/ChunkLoader.h"
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ChunkLoader.h"

#include <fstream>
#include <vector>

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
#include "Plugin/IEditorResourceService.h"
#include "Manager/PluginManager.h"
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
#include "Actor/ActorData.h"

namespace Editor
{
	using Common::IActor;
	using Core::Actor;
	/// @brief Initializes chunk-loading state and actor identifier tracking.
	/// @note Initializes the :ChunkLoader base or delegates to its constructor.
	ChunkLoader::ChunkLoader() : mCurrentChunkData(new Core::ChunkData)
	{
		HMODULE coreMod = GetModuleHandleA(Common::DLLPath::CORE_EDITOR);
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

	/// @brief Releases the resources managed by this instance during destruction.
	ChunkLoader::~ChunkLoader() { delete mCurrentChunkData; }

	/// @brief Serializes the current scene and resources to the selected .chunk file.
	/// @param chunkPath Path to the .chunk file.
	void ChunkLoader::SaveChunk(const char* chunkPath)
	{
		Lock();
		std::ofstream ofs(chunkPath);
		SaveActorsData(ofs);

		Common::FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::RES_DATA);
		if (auto* plugins = Core::GetAvailablePlugins())
		{
			auto iterBegin = plugins->Begin();
			for (auto iter = iterBegin; iter != plugins->End(); ++iter)
			{
				// HashMap iterators visit empty buckets as well as registered plugins.
				if (!*iter || !(*iter)->Value())
					continue;

				void* resService = (*iter)->Value()->QueryInterface(Core::IEditorResourceService::INTERFACE_NAME);
				if (resService)
				{
					Common::FileIOHelper::BeginDataPackSave(ofs, (*iter)->Value()->GetName());
					static_cast<Core::IEditorResourceService*>(resService)->SaveReferencedResources(ofs);
					Common::FileIOHelper::EndDataPackSave(ofs, (*iter)->Value()->GetName());
				}
			}
		}
		Common::FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::RES_DATA);

		ChunkLoader::SaveChunkData(ofs);
		Common::FileIOHelper::SaveBufferToFile(ofs);
		Unlock();
	}

	/// @brief Restores the scene and its resources from a .chunk file.
	/// @param chunkPath Path to the .chunk file.
	void ChunkLoader::LoadChunk(const char* chunkPath)
	{
		Lock();
		std::ifstream ifs(chunkPath);
		ChunkLoader::LoadChunkData(ifs);

		size_t count = Common::FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::RES_DATA).first;
		while (0 < count)
		{
			Common::FTDS::String dllName = Common::FileIOHelper::BeginDataPackLoad(ifs).second;
			if (auto* plugins = Core::GetAvailablePlugins())
			{
				Common::IPlugin* plugin		= plugins->At(dllName)->Value();
				void*			 resService = plugin->QueryInterface(Core::IEditorResourceService::INTERFACE_NAME);
				if (resService)
					static_cast<Core::IEditorResourceService*>(resService)->LoadReferencedResources(ifs);
			}
			--count;
		}

		if (auto* plugins = Core::GetAvailablePlugins())
		{
			for (auto it = plugins->Begin(); it != plugins->End(); ++it)
			{
				if (!*it || !(*it)->Value())
					continue;
				(*it)->Value()->ClearRegisteredComponents();
			}
		}

		LoadActorsData(ifs);

		Core::PluginManager::GetInstance()->Initialize();
		Core::PluginManager::GetInstance()->InitializeComponents();
		Core::PluginManager::GetInstance()->Setup();

		// D3D11::Camera::GetInstance()->LoadProperties(ifs);
		Unlock();
	}

	/// @brief Writes actor properties and components for the current scene.
	/// @param ofs Output stream receiving the serialized data.
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

	/// @brief Restores serialized actor state for the loaded scene.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void ChunkLoader::LoadActorsData(std::ifstream& ifs)
	{
		EditorScene*							  scene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::pair<size_t, Common::FTDS::String>&& pack	= Common::FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::ACTOR_DATA);
		std::vector<Actor*>						  actorBuf;
		ResetMaxActorID();

		for (size_t i = 0; i < pack.first; ++i)
		{
			std::pair<size_t, Common::FTDS::String>&& actorData = Common::FileIOHelper::BeginDataPackLoad(ifs);
			Actor									  actor		= Actor(Common::ChunkKey::ID::INVALID);
			actor.LoadProperties(ifs);
			actor.LoadComponents(ifs);
			Core::IActor* element = scene->AddEditorElement(&actor);
			element->GetTransform()->SetOwner(element);

			// The editor element now owns the parent/child placeholders.
			if (actor.GetChildActors())
				actor.GetChildActors()->Clear();
			if (actor.GetData())
				actor.GetData()->Parent = nullptr;

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

		struct HierarchyLink
		{
			EditorElement*				 Element = nullptr;
			EditorElement*				 Parent	 = nullptr;
			std::vector<EditorElement*> Children;
		};
		std::vector<HierarchyLink> links;
		links.reserve(actors->GetSize());

		for (auto actor = actors->Begin(); actor != actors->End(); ++actor)
		{
			EditorElement* element = static_cast<EditorElement*>(*actor);
			if (!element)
				continue;

			HierarchyLink link;
			link.Element = element;

			if (Common::IActor* placeholder = element->GetParent())
			{
				Common::FTDS::Record<EditorElement*>* record = actorWithIDs.At(placeholder->GetID());
				link.Parent = record ? record->Value() : nullptr;
				if (placeholder->GetChildActors())
					placeholder->GetChildActors()->Clear();
				element->GetData()->Parent = nullptr;
				delete placeholder;
			}

			Common::FTDS::DynamicArray<IActor*>* storedChildren = element->GetChildActors();
			if (storedChildren && 0 < storedChildren->GetSize())
			{
				for (size_t childIndex = 0; childIndex < storedChildren->GetSize(); ++childIndex)
				{
					IActor* placeholder = storedChildren->At(childIndex);
					if (!placeholder)
						continue;

					Common::FTDS::Record<EditorElement*>* record = actorWithIDs.At(placeholder->GetID());
					if (record && record->Value())
						link.Children.push_back(record->Value());
					if (placeholder->GetChildActors())
						placeholder->GetChildActors()->Clear();
					delete placeholder;
				}
				storedChildren->Clear();
			}

			links.push_back(std::move(link));
		}

		for (HierarchyLink& link : links)
		{
			if (link.Parent)
				link.Element->SetParent(link.Parent);
			for (EditorElement* child : link.Children)
				link.Element->AddChild(child);
		}
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

	/// @brief Returns the chunk data used by this chunk loader.
	/// @return Borrowed access to the chunk data.
	Core::ChunkData* ChunkLoader::GetChunkData()
	{
		return mCurrentChunkData;
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

	/// @brief Returns the dllpath used by this service.
	/// @param mod Loaded module containing the plugin entry point.
	/// @param out Receives the operation's output.
	static void GetDLLPath(HMODULE mod, Common::FTDS::String& out)
	{
		char path[MAX_PATH];
		if (GetModuleFileNameA(mod, path, MAX_PATH))
			out.Assign(path);
		else
			std::cerr << "Failed to get path. Error: " << GetLastError() << std::endl;
	}

	/// @brief Returns the editor chunk loader used by this service.
	/// @return Borrowed access to the editor chunk loader.
	EDITOR_API Editor::ChunkLoader* GetEditorChunkLoader()
	{
		return Editor::ChunkLoader::GetInstance();
	}
} // namespace Editor
namespace Editor
{
	/// @brief Pauses game updates while chunk state is being changed.
	void ChunkLoader::Lock() { mCurrentChunkData->IsLoading = true; }
	/// @brief Resumes game updates after chunk processing completes.
	void ChunkLoader::Unlock() { mCurrentChunkData->IsLoading = false; }
	/// @brief Creates a temporary copy of a chunk so the source remains available during loading.
	/// @param out Receives the operation's output.
	/// @param path Filesystem path of the resource or project.
	void ChunkLoader::CopyChunk(Common::FTDS::String& out, const char* path) { Core::ChunkLoader::GetInstance()->CopyChunk(out, path); }
	/// @brief Removes the temporary chunk copy created for loading.
	void ChunkLoader::DeleteCopiedChunk() { Core::ChunkLoader::GetInstance()->DeleteCopiedChunk(); }
	/// @brief Writes the chunk-specific scene metadata to the output stream.
	/// @param out Receives the operation's output.
	void ChunkLoader::SaveChunkData(std::ofstream& out)
	{
		Common::FileIOHelper::BeginDataPackSave(out, Core::ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::SaveInt(out, Core::ChunkKey::ACTOR_COUNT, mCurrentChunkData->MaxActorID);
		Common::FileIOHelper::EndDataPackSave(out, Core::ChunkKey::CHUNK_DATA);
	}
	/// @brief Reads chunk-specific scene metadata from the input stream.
	/// @param in Input value.
	void ChunkLoader::LoadChunkData(std::ifstream& in)
	{
		Common::FileIOHelper::BeginDataPackLoad(in, Core::ChunkKey::CHUNK_DATA);
		Common::FileIOHelper::LoadInt(in, mCurrentChunkData->MaxActorID);
	}
} // namespace Editor
