#include "ResourceManager.h"

#include "EditorChunkLoader.h"
#include "DirectoryHelper.h"
#include "EditorUtils.h"

#include "FTDS/Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

#include "ResourceSystem/SupportedResources.h"
#include "Plugin/IPlugin.h"

#include "DLLData.h"
#include <Core/src/Manager/ResourceManager.h>

namespace Editor
{
	using namespace Core;
	using namespace D3D11;
	// Number of attempts to load resources aborted during the last loading.
	constexpr size_t MAX_ABORTED_COUNT = 50;

	void ResourceManager::SaveResourcesToChunk(std::ofstream& ofs)
	{
		D3D11::ResourceManager* D3D11Res = mGetD3D11ResManagerFunc();

		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::RESOURCE_DATA);

		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::FTPremade::FT_PREMADE);
		SaveResource<Core::FTPremade>(mCoreRes, ofs);
		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::FTPremade::FT_PREMADE);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTFont::FTFONT);
		SaveResource<FTFont>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTFont::FTFONT);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		SaveResource<FTSpineAnimation>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
		SaveResource<FTSpriteAnimation>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTTileMap::FT_TILEMAP);
		SaveResource<FTTileMap>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTTileMap::FT_TILEMAP);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTTexture::FT_TEXTURE);
		SaveResource<FTSprite>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTTexture::FT_TEXTURE);

		FileIOHelper::BeginDataPackSave(ofs, D3D11::ChunkKey::FTMeshGroup::FT_MESH_GROUP);
		SaveResource<FTMeshGroup>(D3D11Res, ofs);
		FileIOHelper::EndDataPackSave(ofs, D3D11::ChunkKey::FTMeshGroup::FT_MESH_GROUP);

		// FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::Sound::SOUND);
		// SaveResource<Sound>(ofs);
		// FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::Sound::SOUND);

		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::JSON::JSON);
		SaveResource<FTJSON>(mCoreRes, ofs);
		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::JSON::JSON);

		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::CSV::CSV);
		SaveResource<FTCSV>(mCoreRes, ofs);
		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::CSV::CSV);

		FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::FTText::FT_TEXT);
		SaveResource<FTText>(mCoreRes, ofs);
		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::FTText::FT_TEXT);

		FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::RESOURCE_DATA);
	}

	void ResourceManager::LoadAllResourcesInAsset()
	{
		const char* pathToAsset = DirectoryHelper::GetInstance()->GetAssetPath().C_Str();
		// File paths whose loading was aborted.
		// Mostly when certain fields are nullptr or not loaded yet.
		FTDS::DynamicArray<FTDS::String*> aborted;

		DirectoryHelper::IterateForFileRecurse(
			pathToAsset,
			[&](std::string path) {
				LoadCoreResByType(path.c_str(), aborted);
				LoadD3D11ResByType(path.c_str(), aborted);
			});

		while (!aborted.IsEmpty())
		{
			aborted.IterateArray([&](FTDS::String* path) {
				if (path)
				{
					LoadCoreResByType(path->C_Str(), aborted);
					LoadD3D11ResByType(path->C_Str(), aborted);
				}
			});
		}

		// ResourceManager::GetInstance()->LoadMaterials();
		LoadDefaultResources();

		aborted.IterateArray([&](FTDS::String* path) {
			delete path;
			path = nullptr;
		});
	}

	void ResourceManager::LoadCoreResByType(const char* filePath, FTDS::DynamicArray<FTDS::String*>& aborted)
	{
		FTDS::String path(filePath);
		size_t		 type = GetCoreTypeIdx(path);

		Core::ResourceManager* resManager = mGetCoreResManagerFunc();
		FTResource*			   res		  = nullptr;
		printf("Loading file... %s\n", filePath);

		switch (type)
		{
			// Loads non-Graphics resource.
			case Core::ResType::PREMADE:
				res = LoadResource<Core::FTPremade>(resManager, path);
				break;
			case Core::ResType::CSV:
				res = LoadResource<Core::FTCSV>(resManager, path);
				break;
			case Core::ResType::JSON:
				res = LoadResource<Core::FTJSON>(resManager, path);
				break;
			case Core::ResType::TEXT:
				res = LoadResource<Core::FTText>(resManager, path);
				break;

			// Filters out the rest of file types.
			case Core::ResType::UNSUPPORTED:
				printf("File %s is unsupported\n", filePath);
				return;
			default:
				return;
		}

		if (res)
		{
			int pos = aborted.Find(&path);
			if (pos != -1)
			{
				delete aborted.At(pos);
				aborted.Erase(pos);
			}
		}
		else
			aborted.PushBack(DBG_NEW FTDS::String(path));
	}

	void ResourceManager::LoadD3D11ResByType(const char* filePath, Core::FTDS::DynamicArray<Core::FTDS::String*>& aborted)
	{
		FTDS::String path(filePath);
		size_t		 type = GetCoreTypeIdx(path);

		D3D11Resource* res = nullptr;
		printf("Loading file... %s\n", filePath);

		switch (type)
		{
			// Loads Graphics resource.
			case D3D11::ResType::TEXTURE:
			{
				FTDS::String  fileName = ExtractFileName(path.C_Str());
				FTResourceDef resDef{ fileName, path };
				FTTexture*	  tex = LoadResource<FTTexture>(path, mRenderer);
				static_cast<FTSprite*>(res)->SetTexture(tex);
				res = tex;
				break;
			}
			case D3D11::ResType::SPRITE_ANIMATION:
				res = LoadResource<FTSpriteAnimation>(path, mRenderer);
				break;
			case D3D11::ResType::SPINE_ANIMATION:
				res = LoadResource<FTSpineAnimation>(path, mRenderer);
				break;
			case D3D11::ResType::MESH_GROUP:
				res = LoadResource<FTMeshGroup>(path, mRenderer);
				break;
			case D3D11::ResType::FONT:
				res = LoadResource<FTFont>(path, mRenderer);
				break;
			case D3D11::ResType::TILEMAP:
				res = LoadResource<FTTileMap>(path, mRenderer);
				break;

			// Filters out the rest of file types.
			case D3D11::ResType::UNSUPPORTED:
				printf("File %s is unsupported\n", filePath);
				return;
			default:
				return;
		}

		if (res)
		{
			int pos = aborted.Find(&path);
			if (pos != -1)
			{
				delete aborted.At(pos);
				aborted.Erase(pos);
			}
		}
		else
			aborted.PushBack(DBG_NEW FTDS::String(path));
	}

	void ResourceManager::PassLoadResourceInChunk(std::ifstream& ifs)
	{
		std::pair<size_t, FTDS::String> resPack	  = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::RESOURCE_DATA);
		size_t							packCount = resPack.first;

		ResArray* mCoreRes = mGetCoreResManagerFunc()->GetResArray();
		ResArray* d3d11Res = mGetD3D11ResManagerFunc()->GetResArray();

		std::pair<size_t, FTDS::String>
			desc = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::FTText::FT_TEXT);
		LoadDummyResource<FTText>(ifs, mCoreRes, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::CSV::CSV);
		LoadDummyResource<FTCSV>(ifs, mCoreRes, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::JSON::JSON);
		LoadDummyResource<FTJSON>(ifs, mCoreRes, desc.first);

		/*	desc = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::Sound::SOUND);
			LoadDummyResource<Sound>(ifs, desc.first);*/

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTMeshGroup::FT_MESH_GROUP);
		LoadDummyResource<FTMeshGroup>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTTexture::FT_TEXTURE);
		LoadDummyResource<FTSprite>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTTileMap::FT_TILEMAP);
		LoadDummyResource<FTTileMap>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
		LoadDummyResource<FTSpriteAnimation>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		LoadDummyResource<FTSpineAnimation>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, D3D11::ChunkKey::FTFont::FTFONT);
		LoadDummyResource<FTFont>(ifs, d3d11Res, desc.first);

		desc = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::FTPremade::FT_PREMADE);
		LoadDummyResource<Core::FTPremade>(ifs, mCoreRes, desc.first);
	}

	void ResourceManager::UpdateUI()
	{
		if (ImGui::Button("Import"))
		{
			IGFD::FileDialogConfig config;
			config.countSelectionMax = 0;
			config.path				 = DirectoryHelper::GetInstance()->GetAssetPath().C_Str();

			ImGuiFileDialog::Instance()->OpenDialog("ImportRes", "Choose Files", FileTypes::ALL_FILE_FORMATS, config);
		}

		/*if (ImGuiFileDialog::Instance()->Display("ImportRes"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::map<std::string, std::string> selection = ImGuiFileDialog::Instance()->GetSelection();
				for (auto iter = selection.begin(); iter != selection.end(); ++iter)
					LoadResByType((*iter).second.c_str(), nullptr);
			}
			ImGuiFileDialog::Instance()->Close();
		}*/

		if (ImGui::Button("Refresh"))
		{
			// EditorSceneManager::GetInstance()->DeleteAll();
			ResourceManager::GetInstance()->DeleteAll();
			LoadAllResourcesInAsset();
			EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
		}

		DisplayLoadedResources<D3D11PSO>("PSOs", GetPSOs());
		DisplayLoadedResources<FTSprite>("Sprites", GetSprites());
		DisplayLoadedResources<FTTileMap>("Tilemaps", GetTileMaps());
		DisplayLoadedResources<FTPremade>("Premades", GetPremades());
		DisplayLoadedResources<FTMaterial>("Materials", GetMaterials());
		DisplayLoadedResources<FTMeshGroup>("MeshGroups", GetMeshGroups());
		DisplayLoadedResources<FTSpriteAnimation>("Sprite Animations", GetSpriteAnimations());
		DisplayLoadedResources<FTSpineAnimation>("Spine Animations", GetSpineAnimations());
		DisplayLoadedResources<Sound>("Sounds", GetSounds());
		DisplayLoadedResources<FTCSV>("CSVs", GetCSVs());
		DisplayLoadedResources<FTJSON>("JSONs", GetJSONs());
		DisplayLoadedResources<FTText>("Texts", GetTexts());
	}

	void ResourceManager::SetRenderer(Editor::EditorRenderer* renderer)
	{
		mRenderer = renderer;
	}

	size_t ResourceManager::GetCoreTypeIdx(FTDS::String& fileName)
	{
		size_t length = 0;
		if (fileName.RFind(".") < 0)
			return Core::ResType::UNSUPPORTED;
		length = fileName.GetLength() - static_cast<size_t>(fileName.RFind("."));

		FTDS::String format = fileName;
		format.SubStr(fileName.RFind("."), length);

		if (StrContains(FileTypes::CSV, format))
			return Core::ResType::CSV;

		else if (StrContains(FileTypes::JSON, format))
			return Core::ResType::JSON;

		else if (StrContains(FileTypes::TEXT, format))
			return Core::ResType::TEXT;

		else if (StrContains(FileTypes::PREMADE, format))
			return Core::ResType::PREMADE;

		else
			return Core::ResType::UNSUPPORTED;
	}

	size_t ResourceManager::GetD3D11TypeIdx(FTDS::String& fileName)
	{
		size_t length = 0;
		if (fileName.RFind(".") < 0)
			return Core::ResType::UNSUPPORTED;
		length = fileName.GetLength() - static_cast<size_t>(fileName.RFind("."));

		FTDS::String format = fileName;
		format.SubStr(fileName.RFind("."), length);

		if (StrContains(FileTypes::TEXTURE, format))
			return D3D11::ResType::TEXTURE;
		else if (StrContains(FileTypes::TILEMAP, format))
			return D3D11::ResType::TILEMAP;
		else if (StrContains(FileTypes::MESH, format))
			return D3D11::ResType::MESH_GROUP;

		else if (StrContains(FileTypes::SPRITE_ANIMATION, format))
			return D3D11::ResType::SPRITE_ANIMATION;

		else if (StrContains(FileTypes::SPINE_ANIMATION, format))
			return D3D11::ResType::SPINE_ANIMATION;

		else if (StrContains(FileTypes::SHADER_META, format))
			return D3D11::ResType::SHADER_META;

		else if (StrContains(FileTypes::SHADER, format))

			if (fileName.Contains(FileTypes::VERTEX_SHADER))
				return D3D11::ResType::VERTEX_SHADER;
			else if (fileName.Contains(FileTypes::PIXEL_SHADER))
				return D3D11::ResType::PIXEL_SHADER;
			else if (fileName.Contains(FileTypes::GEOMETRY_SHADER))
				return D3D11::ResType::GEOMETRY_SHADER;
			else
				return D3D11::ResType::UNSUPPORTED;

		else if (StrContains(FileTypes::FONT, format))
			return D3D11::ResType::FONT;

		else
			return D3D11::ResType::UNSUPPORTED;
	}

	ResourceManager::ResourceManager()
	{
		mCoreRes  = GetFunc<Core::GET_RES_MANAGER_INST>(DLLPaths::CORE_EDITOR, Core::GET_RES_FUNC)();
		mD3D11Res = GetFunc<D3D11::GET_RES_MANAGER_INST>(DLLPaths::D3D11_EDITOR, Core::GET_RES_FUNC)();
	}

	ResourceManager::~ResourceManager()
	{
	}
} // namespace Editor