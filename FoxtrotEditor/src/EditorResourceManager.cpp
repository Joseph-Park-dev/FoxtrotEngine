#include "EditorResourceManager.h"

#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include <ResourceSystem/FTMeshGroup.h>
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/GenericData/FTCSV.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "ResourceSystem/FTMaterials/RimMaterial.h"
#include "ResourceSystem/Sound/Sound.h"

#include "EditorChunkLoader.h"
#include "DirectoryHelper.h"
#include "EditorUtils.h"

#include "Utils/StrAssign.h"
#include "Static/FTString.h"

// Number of attempts to load resources aborted during the last loading.
constexpr size_t RESOURCE_IMPORT_ATTEMPT = 3;

void EditorResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	ResourceManager::GetInstance()->Initialize(renderer);
}

void EditorResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::RESOURCE_DATA);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPremade::FT_PREMADE);
	SaveResourceToChunk<FTPremade>(ofs, ResourceManager::GetInstance()->GetPremades());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPremade::FT_PREMADE);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
	SaveResourceToChunk<FTSpineAnimation>(ofs, ResourceManager::GetInstance()->GetSpineAnimations());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, ResourceManager::GetInstance()->GetSpriteAnimations());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);
	SaveResourceToChunk<FTTileMap>(ofs, ResourceManager::GetInstance()->GetTileMaps());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTexture::FT_TEXTURE);
	SaveResourceToChunk<FTTexture>(ofs, ResourceManager::GetInstance()->GetTextures());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTexture::FT_TEXTURE);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMeshGroup::FT_MESH_GROUP);
	SaveResourceToChunk<FTMeshGroup>(ofs, ResourceManager::GetInstance()->GetMeshGroups());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMeshGroup::FT_MESH_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
	SaveResourceToChunk<FTVertexShader>(ofs, ResourceManager::GetInstance()->GetVertexShaders());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	SaveResourceToChunk<FTPixelShader>(ofs, ResourceManager::GetInstance()->GetPixelShaders());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Sound::SOUND);
	SaveResourceToChunk<Sound>(ofs, ResourceManager::GetInstance()->GetSounds());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::Sound::SOUND);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	SaveResourceToChunk<FTJSON>(ofs, ResourceManager::GetInstance()->GetJSONs());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	SaveResourceToChunk<FTCSV>(ofs, ResourceManager::GetInstance()->GetCSVs());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void EditorResourceManager::LoadAllResourcesInAsset()
{
	GetTextures()->Reserve(20);
	GetTileMaps()->Reserve(20);
	GetPremades()->Reserve(20);
	GetSpriteAnimations()->Reserve(20);
	GetSpineAnimations()->Reserve(20);
	GetMeshGroups()->Reserve(20);
	GetVertexShaders()->Reserve(20);
	GetPixelShaders()->Reserve(20);
	GetMaterials()->Reserve(20);
	GetSounds()->Reserve(20);
	GetCSVs()->Reserve(20);
	GetJSONs()->Reserve(20);
	GetTexts()->Reserve(20);

	const char* pathToAsset = ResourceManager::GetInstance()->GetPathToAsset().C_Str();

	// File paths whose loading was aborted.
	// Mostly when certain fields are nullptr or not loaded yet.
	FTDS::DynamicArray<FTDS::String*> aborted;

	DirectoryHelper::IterateForFileRecurse(
		pathToAsset,
		[&](std::string path) { LoadResByType(path.c_str(), aborted); });

	while (!aborted.IsEmpty())
	{
		aborted.IterateArray([&](FTDS::String* path) {
			if (path)
				LoadResByType(path->C_Str(), aborted);
		});
	}

	ResourceManager::GetInstance()->LoadMaterials();
	ResourceManager::GetInstance()->LoadDefaultResources();

	aborted.IterateArray([&](FTDS::String* path) {
		delete path;
		path = nullptr;
	});
}

void EditorResourceManager::LoadResByType(const char* filePath, FTDS::DynamicArray<FTDS::String*>& aborted)
{
	FTDS::String path(filePath);
	ResType		 type = GetResType(path);

	FTResource* res = nullptr;
	printf("Loading file... %s\n", filePath);

	switch (type)
	{
		// Loads Graphics resource.
		case ResType::FTTEXTURE:
			res = LoadResource(path, GetTextures(), ResourceManager::GetInstance()->GetRenderer());
			break;
		case ResType::FT_SPRITE_ANIMATION:
			res = LoadResource(path, GetSpriteAnimations(), ResourceManager::GetInstance()->GetRenderer());
			break;
		case ResType::FT_SPINE_ANIMATION:
			res = LoadResource(path, GetSpineAnimations(), ResourceManager::GetInstance()->GetRenderer());
			break;
		case ResType::FT_VERTEX_SHADER:
			res = LoadResource(path, GetVertexShaders(), ResourceManager::GetInstance()->GetRenderer());
			break;
		case ResType::FT_PIXEL_SHADER:
			res = LoadResource(path, GetPixelShaders(), ResourceManager::GetInstance()->GetRenderer());
			break;
		case ResType::FTMESH:
			res = LoadResource(path, GetMeshGroups(), ResourceManager::GetInstance()->GetRenderer());
			break;

		// Loads non-Graphics resource.
		case ResType::FTPREMADE:
			res = LoadResource(path, GetPremades());
			break;
		case ResType::FTCSV:
			res = LoadResource(path, GetCSVs());
			break;
		case ResType::FTJSON:
			res = LoadResource(path, GetJSONs());
			break;
		case ResType::FTTEXT:
			res = LoadResource(path, GetTexts());
			break;
		case ResType::FTSOUND:
			res = LoadResource(path, GetSounds());
			break;
		case ResType::FTTILEMAP:
			res = LoadResource(path, GetTileMaps());
			break;

		// Filters out the rest of file types.
		case ResType::UNSUPPORTED:
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

void EditorResourceManager::PassLoadResourceInChunk(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String> resPack	  = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t							packCount = resPack.first;

	std::pair<size_t, FTDS::String> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	LoadDummyResource<FTCSV>(ifs, GetCSVs(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	LoadDummyResource<FTJSON>(ifs, GetJSONs(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Sound::SOUND);
	LoadDummyResource<Sound>(ifs, GetSounds(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	LoadDummyResource<FTPixelShader>(ifs, GetPixelShaders(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
	LoadDummyResource<FTVertexShader>(ifs, GetVertexShaders(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMeshGroup::FT_MESH_GROUP);
	LoadDummyResource<FTMeshGroup>(ifs, GetMeshGroups(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
	LoadDummyResource<FTSpriteAnimation>(ifs, GetSpriteAnimations(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
	LoadDummyResource<FTSpineAnimation>(ifs, GetSpineAnimations(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPremade::FT_PREMADE);
	LoadDummyResource<FTPremade>(ifs, GetPremades(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTileMap::FT_TILEMAP);
	LoadDummyResource<FTTileMap>(ifs, GetTileMaps(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTexture::FT_TEXTURE);
	LoadDummyResource<FTTexture>(ifs, GetTextures(), desc.first);
}

FTDS::HashMap<FTTexture*>* EditorResourceManager::GetTextures()
{
	return ResourceManager::GetInstance()->GetTextures();
}

FTDS::HashMap<FTTileMap*>* EditorResourceManager::GetTileMaps()
{
	return ResourceManager::GetInstance()->GetTileMaps();
}

FTDS::HashMap<FTPremade*>* EditorResourceManager::GetPremades()
{
	return ResourceManager::GetInstance()->GetPremades();
}

FTDS::HashMap<FTVertexShader*>* EditorResourceManager::GetVertexShaders()
{
	return ResourceManager::GetInstance()->GetVertexShaders();
}

FTDS::HashMap<FTPixelShader*>* EditorResourceManager::GetPixelShaders()
{
	return ResourceManager::GetInstance()->GetPixelShaders();
}

FTDS::HashMap<FTMaterial*>* EditorResourceManager::GetMaterials()
{
	return ResourceManager::GetInstance()->GetMaterials();
}

FTDS::HashMap<FTMeshGroup*>* EditorResourceManager::GetMeshGroups()
{
	return ResourceManager::GetInstance()->GetMeshGroups();
}

FTDS::HashMap<FTSpriteAnimation*>* EditorResourceManager::GetSpriteAnimations()
{
	return ResourceManager::GetInstance()->GetSpriteAnimations();
}

FTDS::HashMap<FTSpineAnimation*>* EditorResourceManager::GetSpineAnimations()
{
	return ResourceManager::GetInstance()->GetSpineAnimations();
}

FTDS::HashMap<Sound*>* EditorResourceManager::GetSounds()
{
	return ResourceManager::GetInstance()->GetSounds();
}

FTDS::HashMap<FTCSV*>* EditorResourceManager::GetCSVs()
{
	return ResourceManager::GetInstance()->GetCSVs();
}

FTDS::HashMap<FTJSON*>* EditorResourceManager::GetJSONs()
{
	return ResourceManager::GetInstance()->GetJSONs();
}

FTDS::HashMap<FTText*>* EditorResourceManager::GetTexts()
{
	return ResourceManager::GetInstance()->GetTexts();
}

void EditorResourceManager::UpdateUI()
{
	if (ImGui::Button("Import"))
	{
		IGFD::FileDialogConfig config;
		config.countSelectionMax = 0;
		config.path				 = ResourceManager::GetInstance()->GetPathToAsset().C_Str();

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
		ResourceManager::GetInstance()->DeleteAll();
		LoadAllResourcesInAsset();
	}

	DisplayLoadedResources<FTTexture>("Textures", GetTextures());
	DisplayLoadedResources<FTTileMap>("Tilemaps", GetTileMaps());
	DisplayLoadedResources<FTPremade>("Premades", GetPremades());
	DisplayLoadedResources<FTVertexShader>("Vertex Shaders", GetVertexShaders());
	DisplayLoadedResources<FTPixelShader>("Pixel Shaders", GetPixelShaders());
	DisplayLoadedResources<FTMaterial>("Materials", GetMaterials());
	DisplayLoadedResources<FTMeshGroup>("MeshGroups", GetMeshGroups());
	DisplayLoadedResources<FTSpriteAnimation>("Sprite Animations", GetSpriteAnimations());
	DisplayLoadedResources<FTSpineAnimation>("Spine Animations", GetSpineAnimations());
	DisplayLoadedResources<Sound>("Sounds", GetSounds());
	DisplayLoadedResources<FTCSV>("CSVs", GetCSVs());
	DisplayLoadedResources<FTJSON>("JSONs", GetJSONs());
	DisplayLoadedResources<FTText>("Texts", GetTexts());
}

ResType EditorResourceManager::GetResType(FTDS::String& fileName)
{
	size_t length = 0;
	if (fileName.RFind(".") < 0)
		return ResType::UNSUPPORTED;
	length = fileName.Length() - static_cast<size_t>(fileName.RFind("."));

	FTDS::String format = fileName;
	format.SubStr(fileName.RFind("."), length);

	if (StrContains(FileTypes::TEXTURE, format))
		return ResType::FTTEXTURE;
	else if (StrContains(FileTypes::TILEMAP, format))
		return ResType::FTTILEMAP;
	else if (StrContains(FileTypes::MESH, format))
		return ResType::FTMESH;

	else if (StrContains(FileTypes::SPRITE_ANIMATION, format))
		return ResType::FT_SPRITE_ANIMATION;
	else if (StrContains(FileTypes::SPINE_ANIMATION, format))
		return ResType::FT_SPINE_ANIMATION;

	else if (StrContains(FileTypes::CSV, format))
		return ResType::FTCSV;

	else if (StrContains(FileTypes::JSON, format))
		return ResType::FTJSON;

	else if (StrContains(FileTypes::TEXT, format))
		return ResType::FTTEXT;

	else if (StrContains(FileTypes::SHADER_META, format))
		return ResType::FT_SHADER_META;

	else if (StrContains(FileTypes::SHADER, format))

		if (fileName.Contains(FileTypes::VERTEX_SHADER))
			return ResType::FT_VERTEX_SHADER;
		else if (fileName.Contains(FileTypes::PIXEL_SHADER))
			return ResType::FT_PIXEL_SHADER;
		else
			return ResType::UNSUPPORTED;
	else if (StrContains(FileTypes::Sound::WAV, format))
		return ResType::FTSOUND;

	else
		return ResType::UNSUPPORTED;
}

EditorResourceManager::EditorResourceManager()
{
}

EditorResourceManager::~EditorResourceManager()
{
}