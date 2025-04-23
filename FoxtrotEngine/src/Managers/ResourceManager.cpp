// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "Managers/AnimationManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTSpriteSheet.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/ModelLoader.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "ResourceSystem/FTMaterials/RimMaterial.h"
#include "ResourceSystem/GenericData/FTCSV.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"

#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "imgui/FileDialog/ImGuiFileDialog.h"
	#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

	#include "DirectoryHelper.h"
#endif // FOXTROT_EDITOR

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	mMapTextures.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapTileMaps.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapSpriteSheets.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapPremades.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapSpriteAnimation.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapMeshGroups.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });

	mMapVertexShaders.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapPixelShaders.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapMaterials.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapCSVs.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });
	mMapJSONs.insert({ (UINT)ChunkKey::NullVal::VALUE_NOT_ASSIGNED, nullptr });

	// Add primitive geometries as resources
	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_SQUARE_RED,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(1.0f, 0.0f, 0.0f)), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_SQUARE_GREEN,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(0.0f, 1.0f, 0.0f)), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_SQUARE_BLUE,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(0.0f, 0.0f, 1.0f)), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_BOX,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeBox(), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_SQUARE_GRID,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquareGrid(1.0f, 1.0f, 2, 2), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_CYLINDER,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeCylinder(1.0f, 1.0f, 2, 5), mRenderer) });

	mMapMeshGroups.insert(
		{ ChunkKey::PRIMITIVE_SPHERE,
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSphere(1.0f, 50, 50), mRenderer) });

	mItemKey = mMapMeshGroups.size();

	mMapMeshGroups.at(1)->SetFileName("Primitive Square RED");
	mMapMeshGroups.at(2)->SetFileName("Primitive Square GREEN");
	mMapMeshGroups.at(3)->SetFileName("Primitive Square BLUE");
	mMapMeshGroups.at(4)->SetFileName("Primitive Box");
	mMapMeshGroups.at(5)->SetFileName("Primitive Sqare Grid");
	mMapMeshGroups.at(6)->SetFileName("Primitive Cylinder");
	mMapMeshGroups.at(7)->SetFileName("Primitive Sphere");
}

void ResourceManager::DeleteAll()
{
	ClearMap<FTTexture>(mMapTextures);
	ClearMap<FTTileMap>(mMapTileMaps);
	ClearMap<FTSpriteSheet>(mMapSpriteSheets);
	ClearMap<FTPremade>(mMapPremades);
	ClearMap<FTSpriteAnimation>(mMapSpriteAnimation);
	ClearMap<FTBasicMeshGroup>(mMapMeshGroups);
	ClearMap<FTMaterial>(mMapMaterials);
	ClearMap<FTCSV>(mMapCSVs);
	ClearMap<FTJSON>(mMapJSONs);
}

FTTexture* ResourceManager::GetLoadedTexture(const UINT key)
{
	FTTexture* ptTex = mMapTextures.at(key);
	if (!ptTex)
		printf("Error: Unable to find FTTexture with key; %d\n", key);
	ptTex->AddRefCount();
	return ptTex;
}

FTTexture* ResourceManager::GetLoadedTexture(const char* name)
{
	auto iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); ++iter)
	{
		if ((*iter).second)
		{
			if (FTDS::StringEqual((*iter).second->GetFileName().c_str(), name))
			{
				(*iter).second->AddRefCount();
				return (*iter).second;
			}
		}
	}
	return nullptr;
}

FTTileMap* ResourceManager::GetLoadedTileMap(const UINT key)
{
	FTTileMap* tileMap = mMapTileMaps.at(key);
	if (!tileMap)
		printf("Error: ResourceManager::GetLoadedTileMap() -> FTTileMap is empty %d\n", key);

	tileMap->AddRefCount();
	return tileMap;
}

FTSpriteSheet* ResourceManager::GetLoadedSpriteSheet(const UINT key)
{
	FTSpriteSheet* spriteSheet = mMapSpriteSheets.at(key);
	if (!spriteSheet)
		printf("Error: ResourceManager::GetLoadedTileMap() -> FTTileMap is empty %d\n", key);

	spriteSheet->AddRefCount();
	return spriteSheet;
}

FTPremade* ResourceManager::GetLoadedPremade(const UINT key)
{
	FTPremade* premade = mMapPremades.at(key);
	if (!premade)
		printf("Error: ResourceManager::GetLoadedPremade() -> FTPremade is empty %d\n", key);
	premade->AddRefCount();
	return premade;
}

FTPremade* ResourceManager::GetLoadedPremade(std::string&& fileName)
{
	std::string premadeFullName = fileName + FileTypes::PREMADE;

	std::unordered_map<UINT, FTPremade*>::iterator iter = mMapPremades.begin();
	for (; iter != mMapPremades.end(); ++iter)
	{
		if ((*iter).second->GetFileName() == premadeFullName)
		{
			(*iter).second->AddRefCount();
			return (*iter).second;
		}
	}
	printf("Error: ResourceManager::GetLoadedPremade() -> Cannot find FTPremade %s\n", premadeFullName.c_str());
	return nullptr;
}

FTPixelShader* ResourceManager::GetLoadedPixelShader(const UINT key)
{
	FTPixelShader* shader = mMapPixelShaders.at(key);
	if (!shader)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
	shader->AddRefCount();
	return shader;
}

FTMaterial* ResourceManager::GetLoadedMaterial(const UINT key)
{
	FTMaterial* material = mMapMaterials.at(key);
	if (!material)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
	material->AddRefCount();
	return material;
}

FTBasicMeshGroup* ResourceManager::GetLoadedMesh(const UINT key)
{
	FTBasicMeshGroup* meshGrp = mMapMeshGroups.at(key);
	if (meshGrp)
	{
		mMapMeshGroups.at(key)->AddRefCount();
		return meshGrp;
	}
	printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %d\n", key);
	return nullptr;
}

FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(const UINT key)
{
	FTSpriteAnimation* spriteAnim = mMapSpriteAnimation.at(key);
	if (!spriteAnim)
		printf("Error: ResourceManager::GetLoadedSpriteAnim() -> FTSpriteAnimation is empty %d\n", key);
	spriteAnim->AddRefCount();
	return spriteAnim;
}

FTCSV* ResourceManager::GetLoadedCSV(const UINT key)
{
	FTCSV* csv = mMapCSVs.at(key);
	if (!csv)
		Debug::LogError(__LINE__, __FILE__, "Failed to load FTCSV");
	csv->AddRefCount();
	return csv;
}

FTJSON* ResourceManager::GetLoadedJSON(const UINT key)
{
	FTJSON* json = mMapJSONs.at(key);
	if (!json)
		Debug::LogError(__LINE__, __FILE__, "Failed to load FTCSV");
	json->AddRefCount();
	return json;
}

std::unordered_map<UINT, FTTexture*>& ResourceManager::GetTexturesMap()
{
	return mMapTextures;
}

std::unordered_map<UINT, FTTileMap*>& ResourceManager::GetTileMapsMap()
{
	return mMapTileMaps;
}

std::unordered_map<UINT, FTSpriteSheet*>& ResourceManager::GetSpriteSheetsMap()
{
	return mMapSpriteSheets;
}

std::unordered_map<UINT, FTSpriteAnimation*>& ResourceManager::GetSpriteAnimMap()
{
	return mMapSpriteAnimation;
}

std::unordered_map<UINT, FTBasicMeshGroup*>& ResourceManager::GetMeshGroupsMap()
{
	return mMapMeshGroups;
}

std::unordered_map<UINT, FTVertexShader*>& ResourceManager::GetVertexShadersMap()
{
	return mMapVertexShaders;
}

std::unordered_map<UINT, FTPixelShader*>& ResourceManager::GetPixelShadersMap()
{
	return mMapPixelShaders;
}

std::unordered_map<UINT, FTMaterial*>& ResourceManager::GetMapMaterials()
{
	return mMapMaterials;
}

std::unordered_map<UINT, FTCSV*>& ResourceManager::GetMapCSVs()
{
	return mMapCSVs;
}

std::unordered_map<UINT, FTJSON*>& ResourceManager::GetMapJSONs()
{
	return mMapJSONs;
}

std::string& ResourceManager::GetPathToAsset()
{
	return mPathToAsset;
}

void ResourceManager::SetPathToAsset(std::string&& projectPath)
{
	mPathToAsset.assign(projectPath + "\\Assets\\");
}

void ResourceManager::SaveMaterialsToChunk(std::ofstream& ofs)
{
	typename std::unordered_map<UINT, FTMaterial*>::const_iterator iter;
	for (iter = mMapMaterials.begin(); iter != mMapMaterials.end(); ++iter)
	{
		if ((*iter).second)
			if (0 < (*iter).second->GetRefCount())
				(*iter).second->SaveProperties(ofs, (*iter).first);
	}
}

// void ResourceManager::LoadMaterialsFromChunk(std::ifstream& ifs)
//{
//	FTMaterial* resource = DBG_NEW StandardMaterial;
//	resource->LoadProperties(ifs);
//	mMapMaterials.insert(std::make_pair(mItemKey, resource));
//
//	resource = DBG_NEW RimMaterial;
//	resource->LoadProperties(ifs);
//	mMapMaterials.insert(std::make_pair(mItemKey, resource));
//
//	// Include materials here.
// }

void ResourceManager::LoadMaterials()
{
	UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;

	StandardMaterial* standard				  = DBG_NEW StandardMaterial;
	std::string							 path = std::string(".//Assets//Materials//") + ChunkKey::STANDARD_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path))
		standard->SaveToFile();
	standard->LoadFromFile();

	RimMaterial* rim = DBG_NEW RimMaterial;
	path			 = std::string(".//Assets//Materials//") + ChunkKey::RIM_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path))
		rim->SaveToFile();
	rim->LoadFromFile();

	mMapMaterials.insert({ ++key, standard });
	mMapMaterials.insert({ ++key, rim });
}

void ResourceManager::ProcessTexture(FTTexture* texture)
{
	if (texture->GetIsProcessed())
		return;

	std::string path = texture->GetRelativePath();
	std::string type = ExtractFileType(path.c_str());

	if (type == FileTypes::DDS_TEXTURE)
		DX::ThrowIfFailed(D3D11Utils::CreateCubemapTexture(mRenderer->GetDevice(), texture));
	else
		D3D11Utils::CreateTexture(mRenderer->GetDevice(), mRenderer->GetContext(), texture);

	if (!texture)
		Debug::LogError(__LINE__, __FILE__, "Failed to process Texture.");
	else
		texture->SetIsProcessed(true);
}

void ResourceManager::ProcessSingleMeshGrp(FTBasicMeshGroup* meshGrp)
{
	if (meshGrp->GetIsProcessed())
		return;

	if (meshGrp->GetRelativePath().empty())
		return;
	meshGrp->Initialize(
		GeometryGenerator::ReadFromFile(meshGrp->GetRelativePath()),
		mRenderer->GetDevice(),
		mRenderer->GetContext());

	if (!meshGrp)
		Debug::LogError(__LINE__, __FILE__, "Failed to process MeshGroup.");
	else
		meshGrp->SetIsProcessed(true);
}

void ResourceManager::ProcessTileMap(FTTileMap* tileMap)
{
	if (tileMap->GetIsProcessed())
		return;

	// This if statement will be triggered only on Editor
	// (When loading all assets from Asset folder)
	std::ifstream ifs(tileMap->GetRelativePath());
	tileMap->LoadProperties(ifs);

	tileMap->Initialize();
	tileMap->SetIsProcessed(true);
}

void ResourceManager::ProcessSpriteSheet(FTSpriteSheet* spriteSheet)
{
	if (spriteSheet->GetIsProcessed())
		return;

	// This if statement will be triggered only on Editor
	// (When loading all assets from Asset folder)
	std::ifstream ifs(spriteSheet->GetRelativePath());
	spriteSheet->LoadProperties(ifs);

	spriteSheet->Initialize();
	spriteSheet->SetIsProcessed(true);
}

void ResourceManager::ProcessSpriteAnim(FTSpriteAnimation* spriteAnim)
{
	if (spriteAnim->GetIsProcessed())
		return;

	// This if statement will be triggered only on Editor
	// (When loading all assets from Asset folder)
	if (spriteAnim->GetTileDataKey() == ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		std::ifstream ifs(spriteAnim->GetRelativePath());
		spriteAnim->LoadProperties(ifs);
	}

	FTSpriteSheet* spriteSheet = ResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteAnim->GetTileDataKey());
	spriteAnim->SetTexture();

	std::vector<FTMeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, spriteSheet->GetTiles(), spriteAnim->GetMinFrameIdx(), spriteAnim->GetMaxFrameIdx());
	spriteAnim->Initialize(std::move(meshDataBuf), mRenderer->GetDevice(), mRenderer->GetContext());

	if (!spriteAnim)
		Debug::LogError(__LINE__, __FILE__, "Failed to process SpriteAnimation.");
	else
		spriteAnim->SetIsProcessed(true);
}

void ResourceManager::ProcessCSV(FTCSV* csv)
{
	if (csv->GetIsProcessed())
		return;

	csv->Read();

	if (!csv)
		Debug::LogError(__LINE__, __FILE__, "Failed to process CSV.");
	else
		csv->SetIsProcessed(true);
}

void ResourceManager::ProcessJSON(FTJSON* json)
{
	if (json->GetIsProcessed())
		return;

	json->Read();

	if (!json)
		Debug::LogError(__LINE__, __FILE__, "Failed to process CSV.");
	else
		json->SetIsProcessed(true);
}

void ResourceManager::ProcessMaterial(FTMaterial* material)
{
	for (auto& materialItem : mMapMaterials)
	{
		if (materialItem.second)
		{
			if (materialItem.second->GetIsProcessed())
				continue;

			materialItem.second->LoadFromFile();
			// All loaded premades are included as default.
			materialItem.second->AddRefCount();
			materialItem.second->SetIsProcessed(true);
		}
	}
}

void ResourceManager::ProcessTextures()
{
	for (auto& textureItem : mMapTextures)
		if (textureItem.second)
			ProcessTexture(textureItem.second);
}

void ResourceManager::ProcessMeshGroups()
{
	for (auto& meshGrp : mMapMeshGroups)
		if (meshGrp.second)
			ProcessSingleMeshGrp(meshGrp.second);
}

void ResourceManager::ProcessPremades()
{
	for (auto& premadeItem : mMapPremades)
	{
		if (premadeItem.second)
		{
			premadeItem.second->Load();
			// All loaded premades are included as default.
			premadeItem.second->AddRefCount();
		}
	}
}

void ResourceManager::ProcessTileMaps()
{
	for (auto& tileMapItem : mMapTileMaps)
		if (tileMapItem.second)
			ProcessTileMap(tileMapItem.second);
}

void ResourceManager::ProcessSpriteSheets()
{
	for (auto& spriteSheetItem : mMapSpriteSheets)
		if (spriteSheetItem.second)
			ProcessSpriteSheet(spriteSheetItem.second);
}

void ResourceManager::ProcessSpriteAnims()
{
	for (auto& animMapItem : mMapSpriteAnimation)
		if (animMapItem.second)
			ProcessSpriteAnim(animMapItem.second);
}

void ResourceManager::ProcessCSVs()
{
	for (auto& csvItem : mMapCSVs)
		if (csvItem.second)
			ProcessCSV(csvItem.second);
}

void ResourceManager::ProcessJSONs()
{
	for (auto& jsonItem : mMapJSONs)
		if (jsonItem.second)
			ProcessJSON(jsonItem.second);
}

void ResourceManager::ProcessMaterials()
{
	for (auto& material : mMapMaterials)
		if (material.second)
			ProcessMaterial(material.second);
}

void ResourceManager::ProcessVertexShaders()
{
	for (auto& shader : mMapVertexShaders)
		if (shader.second)
			shader.second->CompileShader(mRenderer);
}

void ResourceManager::ProcessPixelShaders()
{
	for (auto& shader : mMapPixelShaders)
		if (shader.second)
			shader.second->CompileShader(mRenderer);
}

ResourceManager::~ResourceManager()
{
	DeleteAll();
}

ResourceManager::ResourceManager()
	: mItemKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mPathToAsset(".\\Assets\\")
	, mRenderer(nullptr)
{
}

void ResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::RESOURCE_DATA);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);
	SaveResourceToChunk<FTTexture>(ofs, mMapTextures);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);
	SaveResourceToChunk<FTTileMap>(ofs, mMapTileMaps);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);
	SaveResourceToChunk<FTSpriteSheet>(ofs, mMapSpriteSheets);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade>(ofs, mMapPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, mMapSpriteAnimation);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMESH_GROUP);
	SaveResourceToChunk<FTBasicMeshGroup>(ofs, mMapMeshGroups);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMESH_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	SaveResourceToChunk<FTVertexShader>(ofs, mMapVertexShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
	SaveResourceToChunk<FTPixelShader>(ofs, mMapPixelShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	SaveResourceToChunk<FTJSON>(ofs, mMapJSONs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	SaveResourceToChunk<FTCSV>(ofs, mMapCSVs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs, FTCore* ftCoreInst)
{
	DeleteAll();

	std::pair<size_t, std::string> resPack	 = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t						   packCount = resPack.first;

	std::pair<size_t, std::string> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	mMapCSVs.reserve(desc.first);
	LoadResourceFromChunk<FTCSV>(ifs, mMapCSVs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	mMapJSONs.reserve(desc.first);
	LoadResourceFromChunk<FTJSON>(ifs, mMapJSONs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	mMapPixelShaders.reserve(desc.first);
	LoadResourceFromChunk<FTPixelShader>(ifs, mMapPixelShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	mMapVertexShaders.reserve(desc.first);
	LoadResourceFromChunk<FTVertexShader>(ifs, mMapVertexShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	mMapMeshGroups.reserve(mMapMeshGroups.size() + desc.first);
	LoadResourceFromChunk<FTBasicMeshGroup>(ifs, mMapMeshGroups, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	mMapSpriteAnimation.reserve(desc.first);
	LoadResourceFromChunk<FTSpriteAnimation>(ifs, mMapSpriteAnimation, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	mMapPremades.reserve(desc.first);
	LoadResourceFromChunk<FTPremade>(ifs, mMapPremades, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	mMapSpriteSheets.reserve(desc.first);
	LoadResourceFromChunk<FTSpriteSheet>(ifs, mMapSpriteSheets, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	mMapTileMaps.reserve(desc.first);
	LoadResourceFromChunk<FTTileMap>(ifs, mMapTileMaps, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
	mMapTextures.reserve(desc.first);
	LoadResourceFromChunk<FTTexture>(ifs, mMapTextures, desc.first);

	ProcessCSVs();
	ProcessJSONs();

	ProcessTextures();
	ProcessMeshGroups();
	ProcessTileMaps();
	ProcessSpriteSheets();
	ProcessSpriteAnims();

	ProcessMaterials();
	ProcessVertexShaders();
	ProcessPixelShaders();

	LoadMaterials();

	ProcessPremades();
}

#ifdef FOXTROT_EDITOR
void ResourceManager::LoadAllResourcesInAsset()
{
	mItemKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	DirectoryHelper::IterateForFileRecurse(
		mPathToAsset,
		[&](std::string&& path) { LoadResByType(path); });

	ProcessCSVs();
	ProcessJSONs();

	ProcessTextures();
	ProcessMeshGroups();
	ProcessTileMaps();
	ProcessSpriteSheets();
	ProcessSpriteAnims();

	ProcessMaterials();
	ProcessVertexShaders();
	ProcessPixelShaders();

	LoadMaterials();

	ProcessPremades();
}

void ResourceManager::LoadResByType(std::string& filePath)
{
	ResType type = GetResType(filePath);
	printf("Loading file... %s\n", filePath.c_str());
	switch (type)
	{
		case ResType::UNSUPPORTED:
			printf("File %s is unsupported\n", filePath.c_str());
			break;
		case ResType::FTTEXTURE:
			LoadResource(filePath, mMapTextures);
			break;
		case ResType::FTTILEMAP:
			LoadResource(filePath, mMapTileMaps);
			break;
		case ResType::FTSPRITESHEET:
			LoadResource(filePath, mMapSpriteSheets);
			break;
		case ResType::FTPREMADE:
			LoadResource(filePath, mMapPremades);
			break;
		case ResType::FTMESH:
			LoadResource(filePath, mMapMeshGroups);
			break;
		case ResType::FT_SPRITE_ANIMATION:
			LoadResource(filePath, mMapSpriteAnimation);
			break;
		case ResType::FTCSV:
			LoadResource(filePath, mMapCSVs);
			break;
		case ResType::FTJSON:
			LoadResource(filePath, mMapJSONs);
			break;
		case ResType::FT_VERTEX_SHADER:
			LoadResource(filePath, mMapVertexShaders);
			break;
		case ResType::FT_PIXEL_SHADER:
			LoadResource(filePath, mMapPixelShaders);
			break;
		default:
			break;
	}
}

ResType ResourceManager::GetResType(std::string& fileName)
{
	std::string format = fileName.substr(fileName.rfind("."));
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

	else if (StrContains(FileTypes::CSV, format))
		return ResType::FTCSV;

	else if (StrContains(FileTypes::JSON, format))
		return ResType::FTJSON;

	else if (StrContains(FileTypes::SHADER, format))

		if (StrContains(FileTypes::VERTEX_SHADER, fileName))
			return ResType::FT_VERTEX_SHADER;
		else if (StrContains(FileTypes::PIXEL_SHADER, fileName))
			return ResType::FT_PIXEL_SHADER;
		else
			return ResType::UNSUPPORTED;
}

void ResourceManager::UpdateUI()
{
	if (ImGui::Button("Import"))
	{
		IGFD::FileDialogConfig config;
		config.path				 = ".";
		config.countSelectionMax = 1;

		std::string supportedFormat =
			FileTypes::TEXTURE + std::string(", ") + FileTypes::TILEMAP + std::string(", ") + FileTypes::PREMADE;

		ImGuiFileDialog::Instance()->OpenDialog("SelectFile", "Select File", supportedFormat.c_str(), config);
		ImGui::OpenPopup("Select File");
	}
	if (ImGuiFileDialog::Instance()->Display("SelectFile"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string path	  = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string extension = path.substr(path.rfind("."));

			if (StrContains(FileTypes::TEXTURE, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTexture*	texture		 = LoadResource<FTTexture>(relativePath, mMapTextures);
				ProcessTexture(texture);
			}
			else if (StrContains(FileTypes::TILEMAP, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTileMap*	tileMap		 = LoadResource<FTTileMap>(relativePath, mMapTileMaps);
			}
			else if (StrContains(FileTypes::PREMADE, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTPremade*	premade		 = LoadResource<FTPremade>(relativePath, mMapPremades);
				premade->Load();
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::TreeNode("Textures"))
	{
		std::unordered_map<UINT, FTTexture*>::const_iterator texIter;
		texIter = mMapTextures.begin();
		for (texIter = mMapTextures.begin(); texIter != mMapTextures.end(); ++texIter)
		{
			if ((*texIter).second)
			{
				if (ImGui::BeginListBox((*texIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 200)))
				{
					(*texIter).second->UpdateUI();
					if (ImGui::Button("Remove"))
					{
						RemoveResource<FTTexture>((*texIter).first, mMapTextures);
						ImGui::EndListBox();
						break;
					}
					ImGui::EndListBox();
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("TileMaps"))
	{
		std::unordered_map<UINT, FTTileMap*>::const_iterator tileIter;
		tileIter = mMapTileMaps.begin();
		for (; tileIter != mMapTileMaps.end(); ++tileIter)
		{
			if ((*tileIter).second)
			{
				if (ImGui::BeginListBox((*tileIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 200)))
				{
					(*tileIter).second->UpdateUI();
					if (ImGui::Button("Remove"))
					{
						RemoveResource<FTTileMap>((*tileIter).first, mMapTileMaps);
						ImGui::EndListBox();
						break;
					}
					ImGui::EndListBox();
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Premades"))
	{
		std::unordered_map<UINT, FTPremade*>::const_iterator premadeIter;
		premadeIter = mMapPremades.begin();
		for (; premadeIter != mMapPremades.end(); ++premadeIter)
		{
			if ((*premadeIter).second)
			{
				if (ImGui::BeginListBox((*premadeIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 100)))
				{
					(*premadeIter).second->UpdateUI();
					if (ImGui::Button("Remove"))
					{
						RemoveResource<FTPremade>((*premadeIter).first, mMapPremades);
						ImGui::EndListBox();
						break;
					}
					ImGui::EndListBox();
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Vertex Shaders"))
	{
		std::unordered_map<UINT, FTVertexShader*>::const_iterator vsIter;
		vsIter = mMapVertexShaders.begin();
		for (; vsIter != mMapVertexShaders.end(); ++vsIter)
		{
			if (ImGui::BeginListBox((*vsIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 100)))
			{
				(*vsIter).second->UpdateUI();
				if (ImGui::Button("Remove"))
				{
					RemoveResource<FTMaterial>((*vsIter).first, mMapMaterials);
					ImGui::EndListBox();
					break;
				}
				ImGui::EndListBox();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Materials"))
	{
		std::unordered_map<UINT, FTMaterial*>::const_iterator materialIter;
		materialIter = mMapMaterials.begin();
		for (; materialIter != mMapMaterials.end(); ++materialIter)
		{
			if (ImGui::BeginListBox((*materialIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 100)))
			{
				(*materialIter).second->UpdateUI();
				if (ImGui::Button("Remove"))
				{
					RemoveResource<FTMaterial>((*materialIter).first, mMapMaterials);
					ImGui::EndListBox();
					break;
				}
				ImGui::EndListBox();
			}
		}
		ImGui::TreePop();
	}
}
#endif // FOXTROT_EDITOR