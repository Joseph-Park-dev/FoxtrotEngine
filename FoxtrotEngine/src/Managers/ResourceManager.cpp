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

#include "Static/HashChainMap.h"
#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "imgui/FileDialog/ImGuiFileDialog.h"
	#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

	#include "DirectoryHelper.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	mTextures		  = DBG_NEW			FTDS::HashChainMap<FTTexture*>;
	mTileMaps		  = DBG_NEW			FTDS::HashChainMap<FTTileMap*>;
	mSpriteSheets	  = DBG_NEW		FTDS::HashChainMap<FTSpriteSheet*>;
	mPremades		  = DBG_NEW			FTDS::HashChainMap<FTPremade*>;
	mSpriteAnimations = DBG_NEW FTDS::HashChainMap<FTSpriteAnimation*>;
	mMeshGroups		  = DBG_NEW		  FTDS::HashChainMap<FTBasicMeshGroup*>;
	mVertexShaders	  = DBG_NEW	   FTDS::HashChainMap<FTVertexShader*>;
	mPixelShaders	  = DBG_NEW		FTDS::HashChainMap<FTPixelShader*>;
	mMaterials		  = DBG_NEW		   FTDS::HashChainMap<FTMaterial*>;
	mCSVs			  = DBG_NEW				FTDS::HashChainMap<FTCSV*>;
	mJSONs			  = DBG_NEW			   FTDS::HashChainMap<FTJSON*>;
}

void ResourceManager::DeleteAll()
{
	ClearMap(mTextures);
	ClearMap(mTileMaps);
	ClearMap(mSpriteSheets);
	ClearMap(mPremades);
	ClearMap(mSpriteAnimations);
	ClearMap(mMeshGroups);
	ClearMap(mVertexShaders);
	ClearMap(mPixelShaders);
	ClearMap(mMaterials);
	ClearMap(mCSVs);
	ClearMap(mJSONs);
}

FTDS::String& ResourceManager::GetPathToAsset()
{
	return mPathToAsset;
}

void ResourceManager::SetPathToAsset(FTDS::String&& projectPath)
{
	mPathToAsset.Assign(projectPath);
	mPathToAsset.Append("\\Assets\\");
}

FTDS::HashChainMap<FTTexture*>* ResourceManager::GetTextures()
{
	return mTextures;
}

FTDS::HashChainMap<FTTileMap*>* ResourceManager::GetTileMaps()
{
	return mTileMaps;
}

FTDS::HashChainMap<FTSpriteSheet*>* ResourceManager::GetSpriteSheets()
{
	return mSpriteSheets;
}

FTDS::HashChainMap<FTPremade*>* ResourceManager::GetPremades()
{
	return mPremades;
}

FTDS::HashChainMap<FTVertexShader*>* ResourceManager::GetVertexShaders()
{
	return mVertexShaders;
}

FTDS::HashChainMap<FTPixelShader*>* ResourceManager::GetPixelShaders()
{
	return mPixelShaders;
}

FTDS::HashChainMap<FTMaterial*>* ResourceManager::GetMaterials()
{
	return mMaterials;
}

FTDS::HashChainMap<FTBasicMeshGroup*>* ResourceManager::GetMeshGroups()
{
	return mMeshGroups;
}

FTDS::HashChainMap<FTSpriteAnimation*>* ResourceManager::GetSpriteAnimations()
{
	return mSpriteAnimations;
}

FTDS::HashChainMap<FTCSV*>* ResourceManager::GetCSVs()
{
	return mCSVs;
}

FTDS::HashChainMap<FTJSON*>* ResourceManager::GetJSONs()
{
	return mJSONs;
}

void ResourceManager::SaveMaterialsToChunk(std::ofstream& ofs)
{
	for (auto iter = mMaterials->Begin(); iter != mMaterials->End(); ++iter)
	{
		if (iter)
		{
			FTMaterial* mat = (*iter)->Value();
			if (0 < mat->GetRefCount())
				mat->SaveProperties(ofs);
		}
	}
}

void ResourceManager::LoadMaterials()
{
	const char* key	 = ChunkKey::NullVal::NULL_OBJECT;
	size_t		size = 3;

	StandardMaterial* standard				  = DBG_NEW StandardMaterial;
	FTDS::String						 path = FTDS::String(".//Assets//Materials//") + ChunkKey::STANDARD_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path.C_Str()))
		standard->SaveToFile();
	standard->LoadFromFile();

	RimMaterial* rim = DBG_NEW RimMaterial;
	path			 = FTDS::String(".//Assets//Materials//") + ChunkKey::RIM_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path.C_Str()))
		rim->SaveToFile();
	rim->LoadFromFile();

	mMaterials->Reserve(size);

	mMaterials->Insert(standard->FileName(), standard);
	mMaterials->Insert(rim->FileName(), rim);
}

FoxtrotRenderer* ResourceManager::GetRenderer()
{
	return mRenderer;
}

ResourceManager::~ResourceManager()
{
	DeleteAll();

	delete mTextures;
	delete mTileMaps;
	delete mSpriteSheets;
	delete mPremades;
	delete mSpriteAnimations;
	delete mMeshGroups;
	delete mVertexShaders;
	delete mPixelShaders;
	delete mMaterials;
	delete mCSVs;
	delete mJSONs;
}

ResourceManager::ResourceManager()
	: mPathToAsset(".\\Assets\\")
	, mRenderer(nullptr)
	, mTextures(nullptr)
	, mTileMaps(nullptr)
	, mSpriteSheets(nullptr)
	, mPremades(nullptr)
	, mSpriteAnimations(nullptr)
	, mMeshGroups(nullptr)
	, mVertexShaders(nullptr)
	, mPixelShaders(nullptr)
	, mMaterials(nullptr)
	, mCSVs(nullptr)
	, mJSONs(nullptr)
{
}

void ResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::RESOURCE_DATA);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);
	SaveResourceToChunk<FTTexture>(ofs, mTextures);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);
	SaveResourceToChunk<FTTileMap>(ofs, mTileMaps);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);
	SaveResourceToChunk<FTSpriteSheet>(ofs, mSpriteSheets);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade>(ofs, mPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, mSpriteAnimations);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMESH_GROUP);
	SaveResourceToChunk<FTBasicMeshGroup>(ofs, mMeshGroups);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMESH_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	SaveResourceToChunk<FTVertexShader>(ofs, mVertexShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
	SaveResourceToChunk<FTPixelShader>(ofs, mPixelShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	SaveResourceToChunk<FTJSON>(ofs, mJSONs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	SaveResourceToChunk<FTCSV>(ofs, mCSVs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs)
{
	DeleteAll();

	std::pair<size_t, FTDS::String> resPack	  = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t							packCount = resPack.first;

	std::pair<size_t, FTDS::String> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	mCSVs->Reserve(desc.first);
	LoadResourceFromChunk<FTCSV>(ifs, mCSVs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	mJSONs->Reserve(desc.first);
	LoadResourceFromChunk<FTJSON>(ifs, mJSONs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	mPixelShaders->Reserve(desc.first);
	LoadResourceFromChunk<FTPixelShader>(ifs, mPixelShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	mVertexShaders->Reserve(desc.first);
	LoadResourceFromChunk<FTVertexShader>(ifs, mVertexShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	mMeshGroups->Reserve(desc.first);
	LoadResourceFromChunk<FTBasicMeshGroup>(ifs, mMeshGroups, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	mSpriteAnimations->Reserve(desc.first);
	LoadResourceFromChunk<FTSpriteAnimation>(ifs, mSpriteAnimations, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	mPremades->Reserve(desc.first);
	LoadResourceFromChunk<FTPremade>(ifs, mPremades, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	mSpriteSheets->Reserve(desc.first);
	LoadResourceFromChunk<FTSpriteSheet>(ifs, mSpriteSheets, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	mTileMaps->Reserve(desc.first);
	LoadResourceFromChunk<FTTileMap>(ifs, mTileMaps, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
	mTextures->Reserve(desc.first);
	LoadResourceFromChunk<FTTexture>(ifs, mTextures, desc.first);

	ProcessResources(FTCore::GetInstance(), mCSVs);
	ProcessResources(FTCore::GetInstance(), mJSONs);
	ProcessResources(FTCore::GetInstance(), mTextures);
	ProcessResources(FTCore::GetInstance(), mMeshGroups);
	ProcessResources(FTCore::GetInstance(), mTileMaps);
	ProcessResources(FTCore::GetInstance(), mSpriteSheets);
	ProcessResources(FTCore::GetInstance(), mSpriteAnimations);
	ProcessResources(FTCore::GetInstance(), mMaterials);
	ProcessResources(FTCore::GetInstance(), mVertexShaders);
	ProcessResources(FTCore::GetInstance(), mPixelShaders);

	LoadMaterials();

	ProcessResources(FTCore::GetInstance(), mPremades);
}

FTTexture* ResourceManager::GetLoadedTexture(FTDS::String& key)
{
	FTTexture* texture = mTextures->At(key)->Value();
	if (!texture)
		Debug::LogError(__LINE__, __FILE__, "FTTileMap is NULL");
	return texture;
}

FTTileMap* ResourceManager::GetLoadedTileMap(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::TILEMAP);

	FTTileMap* tileMap = mTileMaps->At(key)->Value();
	if (!tileMap)
		Debug::LogError(__LINE__, __FILE__, "FTTileMap is NULL");
	return tileMap;
}

FTSpriteSheet* ResourceManager::GetLoadedSpriteSheet(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SPRITE_SHEET);

	FTSpriteSheet* spriteSheet = mSpriteSheets->At(key)->Value();
	if (!spriteSheet)
		Debug::LogError(__LINE__, __FILE__, "FTSpriteSheet is NULL");
	return spriteSheet;
}

FTPremade* ResourceManager::GetLoadedPremade(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::PREMADE);

	FTPremade* premade = mPremades->At(key)->Value();
	if (!premade)
		Debug::LogError(__LINE__, __FILE__, "FTPremade is NULL");
	return premade;
}

FTVertexShader* ResourceManager::GetLoadedVertexShader(FTDS::String& key)
{
	FTVertexShader* vs = mVertexShaders->At(key)->Value();
	if (!vs)
		Debug::LogError(__LINE__, __FILE__, "VertexShader is NULL");
	return vs;
}

FTPixelShader* ResourceManager::GetLoadedPixelShader(FTDS::String& key)
{
	FTPixelShader* ps = mPixelShaders->At(key)->Value();
	if (!ps)
		Debug::LogError(__LINE__, __FILE__, "PixelShader is NULL");
	return ps;
}

FTMaterial* ResourceManager::GetLoadedMaterial(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::MATERIAL);

	FTMaterial* mat = mMaterials->At(key)->Value();
	if (!mat)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is NULL");
	return mat;
}

FTBasicMeshGroup* ResourceManager::GetLoadedMesh(FTDS::String& key)
{
	FTBasicMeshGroup* meshGrp = mMeshGroups->At(key)->Value();
	if (!meshGrp)
		Debug::LogError(__LINE__, __FILE__, "FTMeshGroup is NULL");
	return meshGrp;
}

FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SPRITE_ANIMATION);

	FTSpriteAnimation* spriteAnim = mSpriteAnimations->At(key)->Value();
	if (!spriteAnim)
		Debug::LogError(__LINE__, __FILE__, "FTSpirteAnimation is NULL");
	return spriteAnim;
}

FTCSV* ResourceManager::GetLoadedCSV(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::CSV);

	FTCSV* ftCSV = mCSVs->At(key)->Value();
	if (!ftCSV)
		Debug::LogError(__LINE__, __FILE__, "FTCSV is NULL");
	return ftCSV;
}

FTJSON* ResourceManager::GetLoadedJSON(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::JSON);

	FTJSON* ftJSON = mJSONs->At(key)->Value();
	if (!ftJSON)
		Debug::LogError(__LINE__, __FILE__, "FTJSON is NULL");
	return ftJSON;
}