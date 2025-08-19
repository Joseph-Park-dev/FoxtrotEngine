#include "EditorResourceManager.h"

#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTSpriteSheet.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
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

void EditorResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	ResourceManager::GetInstance()->Initialize(renderer);
}

void EditorResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::RESOURCE_DATA);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);
	SaveResourceToChunk<FTTexture>(ofs, GetTextures());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);
	SaveResourceToChunk<FTTileMap>(ofs, GetTileMaps());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);
	SaveResourceToChunk<FTSpriteSheet>(ofs, GetSpriteSheets());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade>(ofs, GetPremades());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpineAnimation>(ofs, GetSpineAnimations());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, GetSpriteAnimations());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMESH_GROUP);
	SaveResourceToChunk<FTBasicMeshGroup>(ofs, GetMeshGroups());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMESH_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	SaveResourceToChunk<FTVertexShader>(ofs, GetVertexShaders());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
	SaveResourceToChunk<FTPixelShader>(ofs, GetPixelShaders());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SOUND);
	SaveResourceToChunk<Sound>(ofs, GetSounds());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SOUND);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	SaveResourceToChunk<FTJSON>(ofs, GetJSONs());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	SaveResourceToChunk<FTCSV>(ofs, GetCSVs());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void EditorResourceManager::LoadAllResourcesInAsset()
{
	GetTextures()->Reserve(10);
	GetTileMaps()->Reserve(10);
	GetSpriteSheets()->Reserve(10);
	GetPremades()->Reserve(10);
	GetSpriteAnimations()->Reserve(10);
	GetSpineAnimations()->Reserve(10);
	GetMeshGroups()->Reserve(10);
	GetVertexShaders()->Reserve(10);
	GetPixelShaders()->Reserve(10);
	GetMaterials()->Reserve(10);
	GetSounds()->Reserve(10);
	GetCSVs()->Reserve(10);
	GetJSONs()->Reserve(10);
	GetTexts()->Reserve(10);

	const char* pathToAsset = ResourceManager::GetInstance()->GetPathToAsset().C_Str();

	DirectoryHelper::IterateForFileRecurse(
		pathToAsset,
		[&](std::string&& path) { LoadResByType(path.c_str()); });

	ResourceManager::GetInstance()->LoadMaterials();

	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetCSVs());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetJSONs());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetTextures());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetMeshGroups());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetTileMaps());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetSpriteSheets());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetSpriteAnimations());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetSpineAnimations());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetMaterials());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetVertexShaders());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetPixelShaders());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetSounds());
	ResourceManager::GetInstance()->ProcessResources(FTCoreEditor::GetInstance(), GetPremades());

	ResourceManager::GetInstance()->LoadDefaultResources();
}

void EditorResourceManager::LoadResByType(const char* filePath)
{
	FTDS::String path(filePath);
	ResType		 type = GetResType(path);
	printf("Loading file... %s\n", filePath);
	switch (type)
	{
		case ResType::UNSUPPORTED:
			printf("File %s is unsupported\n", filePath);
			break;
		case ResType::FTTEXTURE:
			LoadResource(path, GetTextures());
			break;
		case ResType::FTTILEMAP:
			LoadResource(path, GetTileMaps());
			break;
		case ResType::FTSPRITESHEET:
			LoadResource(path, GetSpriteSheets());
			break;
		case ResType::FTPREMADE:
			LoadResource(path, GetPremades());
			break;
		case ResType::FTMESH:
			LoadResource(path, GetMeshGroups());
			break;
		case ResType::FT_SPRITE_ANIMATION:
			LoadResource(path, GetSpriteAnimations());
			break;
		case ResType::FT_SPINE_ANIMATION:
			LoadResource(path, GetSpineAnimations());
			break;
		case ResType::FTCSV:
			LoadResource(path, GetCSVs());
			break;
		case ResType::FTJSON:
			LoadResource(path, GetJSONs());
			break;
		case ResType::FTTEXT:
			LoadResource(path, GetTexts());
			break;
		case ResType::FT_VERTEX_SHADER:
			LoadResource(path, GetVertexShaders());
			break;
		case ResType::FT_PIXEL_SHADER:
			LoadResource(path, GetPixelShaders());
			break;
		case ResType::FTSOUND:
			LoadResource(path, GetSounds());
			break;
		default:
			break;
	}
}

void EditorResourceManager::PassLoadResourceInChunk(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String> resPack	  = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t							packCount = resPack.first;

	std::pair<size_t, FTDS::String> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	LoadDummyResource<FTCSV>(ifs, GetCSVs(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	LoadDummyResource<FTJSON>(ifs, GetJSONs(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SOUND);
	LoadDummyResource<Sound>(ifs, GetSounds(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	LoadDummyResource<FTPixelShader>(ifs, GetPixelShaders(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	LoadDummyResource<FTVertexShader>(ifs, GetVertexShaders(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	LoadDummyResource<FTBasicMeshGroup>(ifs, GetMeshGroups(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	LoadDummyResource<FTSpriteAnimation>(ifs, GetSpriteAnimations(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
	LoadDummyResource<FTSpineAnimation>(ifs, GetSpineAnimations(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	LoadDummyResource<FTPremade>(ifs, GetPremades(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	LoadDummyResource<FTSpriteSheet>(ifs, GetSpriteSheets(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	LoadDummyResource<FTTileMap>(ifs, GetTileMaps(), desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
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

FTDS::HashMap<FTSpriteSheet*>* EditorResourceManager::GetSpriteSheets()
{
	return ResourceManager::GetInstance()->GetSpriteSheets();
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

FTDS::HashMap<FTBasicMeshGroup*>* EditorResourceManager::GetMeshGroups()
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
		config.path				 = GetPathToAsset().C_Str();

		ImGuiFileDialog::Instance()->OpenDialog("ImportRes", "Choose Files", FileTypes::ALL_FILE_FORMATS, config);
	}

	if (ImGuiFileDialog::Instance()->Display("ImportRes"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::map<std::string, std::string> selection = ImGuiFileDialog::Instance()->GetSelection();
			for (auto iter = selection.begin(); iter != selection.end(); ++iter)
				LoadResByType((*iter).second.c_str());
		}
		ImGuiFileDialog::Instance()->Close();
	}

	DisplayLoadedResources<FTTexture>("Textures", GetTextures());
	DisplayLoadedResources<FTTileMap>("Tilemaps", GetTileMaps());
	DisplayLoadedResources<FTSpriteSheet>("SpriteSheets", GetSpriteSheets());
	DisplayLoadedResources<FTPremade>("Premades", GetPremades());
	DisplayLoadedResources<FTVertexShader>("Vertex Shaders", GetVertexShaders());
	DisplayLoadedResources<FTPixelShader>("Pixel Shaders", GetPixelShaders());
	DisplayLoadedResources<FTMaterial>("Materials", GetMaterials());
	DisplayLoadedResources<FTBasicMeshGroup>("MeshGroups", GetMeshGroups());
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
	else if (StrContains(FileTypes::SPRITE_SHEET, format))
		return ResType::FTSPRITESHEET;
	else if (StrContains(FileTypes::PREMADE, format))
		return ResType::FTPREMADE;
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

	else if (StrContains(FileTypes::SHADER, format))

		if (fileName.Contains(FileTypes::VERTEX_SHADER))
			return ResType::FT_VERTEX_SHADER;
		else if (fileName.Contains(FileTypes::PIXEL_SHADER))
			return ResType::FT_PIXEL_SHADER;
		else
			return ResType::UNSUPPORTED;
	else if (StrContains(FileTypes::Sound::WAV, format))
		return ResType::FTSOUND;
}

EditorResourceManager::EditorResourceManager()
{
}

EditorResourceManager::~EditorResourceManager()
{
}