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
	ResourceManager::GetTextures()->Reserve(10);
	ResourceManager::GetTileMaps()->Reserve(10);
	ResourceManager::GetSpriteSheets()->Reserve(10);
	ResourceManager::GetPremades()->Reserve(10);
	ResourceManager::GetSpriteAnimations()->Reserve(10);
	ResourceManager::GetSpineAnimations()->Reserve(10);
	ResourceManager::GetMeshGroups()->Reserve(10);
	ResourceManager::GetVertexShaders()->Reserve(10);
	ResourceManager::GetPixelShaders()->Reserve(10);
	ResourceManager::GetMaterials()->Reserve(10);
	ResourceManager::GetSounds()->Reserve(10);
	ResourceManager::GetCSVs()->Reserve(10);
	ResourceManager::GetJSONs()->Reserve(10);
	ResourceManager::GetTexts()->Reserve(10);

	DirectoryHelper::IterateForFileRecurse(
		GetPathToAsset().C_Str(),
		[&](std::string&& path) { LoadResByType(path.c_str()); });

	// ResourceManager::GetTextures()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetTileMaps()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetSpriteSheets()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetPremades()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetSpriteAnimations()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetMeshGroups()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetVertexShaders()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetPixelShaders()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetMaterials()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetCSVs()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);
	// ResourceManager::GetJSONs()->Insert(ChunkKey::NullVal::NULL_OBJECT, nullptr);

	ProcessResources(FTCoreEditor::GetInstance(), GetCSVs());
	ProcessResources(FTCoreEditor::GetInstance(), GetJSONs());
	ProcessResources(FTCoreEditor::GetInstance(), GetTextures());
	ProcessResources(FTCoreEditor::GetInstance(), GetMeshGroups());
	ProcessResources(FTCoreEditor::GetInstance(), GetTileMaps());
	ProcessResources(FTCoreEditor::GetInstance(), GetSpriteSheets());
	ProcessResources(FTCoreEditor::GetInstance(), GetSpriteAnimations());
	ProcessResources(FTCoreEditor::GetInstance(), GetSpineAnimations());
	ProcessResources(FTCoreEditor::GetInstance(), GetMaterials());
	ProcessResources(FTCoreEditor::GetInstance(), GetVertexShaders());
	ProcessResources(FTCoreEditor::GetInstance(), GetPixelShaders());
	ProcessResources(FTCoreEditor::GetInstance(), GetSounds());

	LoadMaterials();

	FTBasicMeshGroup* meshGroup = DBG_NEW FTBasicMeshGroup;
	meshGroup->Initialize(
		{ GeometryGenerator::MakeSquare(1.0f, FTVector3(0.f, 0.f, 1.f)) },
		GetRenderer()->GetDevice(),
		GetRenderer()->GetContext());
	GetMeshGroups()->Insert(ChunkKey::PRIMITIVE_SQUARE_BLUE, meshGroup);

	ProcessResources(FTCoreEditor::GetInstance(), GetPremades());
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

void EditorResourceManager::LoadMaterials()
{
	StandardMaterial* standard = DBG_NEW StandardMaterial;
	RimMaterial* rim		   = DBG_NEW RimMaterial;

	FTDS::String path = FTDS::String(".//Assets//Materials//") + ChunkKey::STANDARD_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path.C_Str()))
		standard->SaveToFile();
	standard->LoadFromFile();

	path = FTDS::String(".//Assets//Materials//") + ChunkKey::RIM_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path.C_Str()))
		rim->SaveToFile();
	rim->LoadFromFile();

	GetMaterials()->Insert(standard->FileName(), standard);
	GetMaterials()->Insert(rim->FileName(), rim);
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
	/*mFileTypeSpecs = DBG_NEW FTDS::DynamicArray<COMDLG_FILTERSPEC*>;
	mFileTypeSpecs->Reserve(FileTypes::MAX_RES_TYPE_COUNT);

	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Premades", FileTypes::PREMADE);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Textures", FileTypes::TEXTURE);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"DDS Textures", FileTypes::DDS_TEXTURE);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Tilemaps", FileTypes::TILEMAP);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"SpriteSheets", FileTypes::SPRITE_SHEET);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"CSVs", FileTypes::CSV);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"JSONs", FileTypes::JSON);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Texts", FileTypes::TEXT);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Meshes", FileTypes::MESH);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Sprite Animation", FileTypes::SPRITE_ANIMATION);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Spine Animation", FileTypes::SPINE_ANIMATION);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Vertex Shaders", FileTypes::VERTEX_SHADER);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Pixel Shaders", FileTypes::PIXEL_SHADER);
	FileTypes::GetFileTypesSpec(mFileTypeSpecs, L"Materials", FileTypes::MATERIAL);*/
}

EditorResourceManager::~EditorResourceManager()
{
	////mFileTypeSpecs->ClearDynamicMem();
	// for (size_t i = 0; i < mFileTypeSpecs->GetSize(); ++i)
	//	delete mFileTypeSpecs->At(i);

	// delete mFileTypeSpecs;
}