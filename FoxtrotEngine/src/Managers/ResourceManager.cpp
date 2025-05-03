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

#include "Static/Array.h"
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
}

void ResourceManager::DeleteAll()
{
	ClearResArray(mTextures);
	ClearResArray(mTileMaps);
	ClearResArray(mSpriteSheets);
	ClearResArray(mPremades);
	ClearResArray(mSpriteAnimations);
	ClearResArray(mMeshGroups);
	ClearResArray(mVertexShaders);
	ClearResArray(mPixelShaders);
	ClearResArray(mMaterials);
	ClearResArray(mCSVs);
	ClearResArray(mJSONs);
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
	for (auto iter = mMaterials.Begin(); iter != mMaterials.End(); ++iter)
	{
		if (iter)
			if (0 < (*iter)->GetRefCount())
				(*iter)->SaveProperties(ofs, mMaterials.IterPos());
	}
}

void ResourceManager::LoadMaterials()
{
	UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	size_t size = 3;

	StandardMaterial* standard = DBG_NEW StandardMaterial;
	std::string							 path = std::string(".//Assets//Materials//") + ChunkKey::STANDARD_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path))
		standard->SaveToFile();
	standard->LoadFromFile();

	RimMaterial* rim = DBG_NEW RimMaterial;
	path = std::string(".//Assets//Materials//") + ChunkKey::RIM_MAT + FileTypes::MATERIAL;
	if (!std::filesystem::exists(path))
		rim->SaveToFile();
	rim->LoadFromFile();

	mMaterials.Reserve(size);
	mMaterials[0] = standard;
	mMaterials[1] = rim;
}

FoxtrotRenderer* ResourceManager::GetRenderer()
{
	return mRenderer;
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

#ifdef FOXTROT_EDITOR
	FTSpriteSheet* spriteSheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteAnim->GetTileDataKey());
#else
	FTSpriteSheet* spriteSheet = ResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteAnim->GetTileDataKey());
#endif // FOXTROT_EDITOR
	
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
	auto iter = mMaterials.Begin();
	for (; iter != mMaterials.End(); ++iter)
	{
		if (iter)
		{
			if ((*iter)->GetIsProcessed())
				continue;

			(*iter)->LoadFromFile();
			// All loaded premades are included as default.
			(*iter)->AddRefCount();
			(*iter)->SetIsProcessed(true);
		}
	}
}

void ResourceManager::ProcessTextures()
{
	for(auto iter = mTextures.Begin(); iter !=mTextures.End(); ++iter)
		if (iter)
			ProcessTexture(*iter);
}

void ResourceManager::ProcessMeshGroups()
{
	for (auto iter = mMeshGroups.Begin(); iter != mMeshGroups.End(); ++iter)
		if (iter)
			ProcessSingleMeshGrp(*iter);
}

void ResourceManager::ProcessPremades()
{
	for (auto iter = mPremades.Begin(); iter != mPremades.End(); ++iter)
		if (iter)
		{
			(*iter)->Load();
			// All loaded premades are included as default.
			(*iter)->AddRefCount();
		}
}

void ResourceManager::ProcessTileMaps()
{
	for (auto iter = mTileMaps.Begin(); iter != mTileMaps.End(); ++iter)
		if (iter)
			ProcessTileMap(*iter);
}

void ResourceManager::ProcessSpriteSheets()
{
	for (auto iter = mSpriteSheets.Begin(); iter != mSpriteSheets.End(); ++iter)
		if (iter)
			ProcessSpriteSheet(*iter);
}

void ResourceManager::ProcessSpriteAnims()
{
	for (auto iter = mSpriteAnimations.Begin(); iter != mSpriteAnimations.End(); ++iter)
		if (iter)
			ProcessSpriteAnim(*iter);
}

void ResourceManager::ProcessCSVs()
{
	for (auto iter = mCSVs.Begin(); iter != mCSVs.End(); ++iter)
		if (iter)
			ProcessCSV(*iter);
}

void ResourceManager::ProcessJSONs()
{
	for (auto iter = mJSONs.Begin(); iter != mJSONs.End(); ++iter)
		if (iter)
			ProcessJSON(*iter);
}

void ResourceManager::ProcessMaterials()
{
	for (auto iter = mMaterials.Begin(); iter != mMaterials.End(); ++iter)
		if (iter)
			ProcessMaterial(*iter);
}

void ResourceManager::ProcessVertexShaders()
{
	for (auto iter = mVertexShaders.Begin(); iter != mVertexShaders.End(); ++iter)
		if (iter)
			(*iter)->CompileShader(mRenderer);
}

void ResourceManager::ProcessPixelShaders()
{
	for (auto iter = mPixelShaders.Begin(); iter != mPixelShaders.End(); ++iter)
		if (iter)
			(*iter)->CompileShader(mRenderer);
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
	SaveResourceToChunk<FTTexture*>(ofs, mTextures);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);
	SaveResourceToChunk<FTTileMap*>(ofs, mTileMaps);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);
	SaveResourceToChunk<FTSpriteSheet*>(ofs, mSpriteSheets);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSPRITESHEET_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade*>(ofs, mPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation*>(ofs, mSpriteAnimations);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMESH_GROUP);
	SaveResourceToChunk<FTBasicMeshGroup*>(ofs, mMeshGroups);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMESH_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	SaveResourceToChunk<FTVertexShader*>(ofs, mVertexShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
	SaveResourceToChunk<FTPixelShader*>(ofs, mPixelShaders);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	SaveResourceToChunk<FTJSON*>(ofs, mJSONs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	SaveResourceToChunk<FTCSV*>(ofs, mCSVs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs, FTCore* ftCoreInst)
{
	DeleteAll();

	std::pair<size_t, std::string> resPack	 = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t						   packCount = resPack.first;

	std::pair<size_t, std::string> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	mCSVs.Reserve(desc.first);
	LoadResourceFromChunk<FTCSV>(ifs, mCSVs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	mJSONs.Reserve(desc.first);
	LoadResourceFromChunk<FTJSON>(ifs, mJSONs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	mPixelShaders.Reserve(desc.first);
	LoadResourceFromChunk<FTPixelShader>(ifs, mPixelShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	mVertexShaders.Reserve(desc.first);
	LoadResourceFromChunk<FTVertexShader>(ifs, mVertexShaders, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	mMeshGroups.Reserve(desc.first);
	LoadResourceFromChunk<FTBasicMeshGroup>(ifs, mMeshGroups, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	mSpriteAnimations.Reserve(desc.first);
	LoadResourceFromChunk<FTSpriteAnimation>(ifs, mSpriteAnimations, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	mPremades.Reserve(desc.first);
	LoadResourceFromChunk<FTPremade>(ifs, mPremades, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	mSpriteSheets.Reserve(desc.first);
	LoadResourceFromChunk<FTSpriteSheet>(ifs, mSpriteSheets, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	mTileMaps.Reserve(desc.first);
	LoadResourceFromChunk<FTTileMap>(ifs, mTileMaps, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
	mTextures.Reserve(desc.first);
	LoadResourceFromChunk<FTTexture>(ifs, mTextures, desc.first);

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

FTTexture* ResourceManager::GetLoadedTexture(const UINT key)
{
	FTTexture* tex = mTextures.At(key);
	if (!tex)
		Debug::LogError(__LINE__, __FILE__, "FTTexture is NULL");
	return tex;
}

FTTexture* ResourceManager::GetLoadedTexture(const char* fileName)
{
	for (auto iter = mTextures.Begin(); iter != mTextures.End(); ++iter)
		if ((*iter)->GetFileName() == fileName)
			return *iter;

	std::string errMsg = std::string("Cannot find FTTexture with ", fileName);
	Debug::LogError(__LINE__, __FILE__, errMsg.c_str());
	return nullptr;
}

FTTileMap* ResourceManager::GetLoadedTileMap(const UINT key)
{
	FTTileMap* tileMap = mTileMaps.At(key);
	if (!tileMap)
		Debug::LogError(__LINE__, __FILE__, "FTTileMap is NULL");
	return tileMap;
}

FTSpriteSheet* ResourceManager::GetLoadedSpriteSheet(const UINT key)
{
	FTSpriteSheet* spriteSheet = mSpriteSheets.At(key);
	if (!spriteSheet)
		Debug::LogError(__LINE__, __FILE__, "FTSpriteSheet is NULL");
	return spriteSheet;
}

FTPremade* ResourceManager::GetLoadedPremade(const UINT key)
{
	FTPremade* premade = mPremades.At(key);
	if (!premade)
		Debug::LogError(__LINE__, __FILE__, "FTPremade is NULL");
	return premade;
}

FTPremade* ResourceManager::GetLoadedPremade(const char* fileName)
{
	for (auto iter = mPremades.Begin(); iter != mPremades.End(); ++iter)
		if ((*iter)->GetFileName() == fileName)
			return *iter;

	std::string errMsg = std::string("Cannot find FTPremade with ", fileName);
	Debug::LogError(__LINE__, __FILE__, errMsg.c_str());
	return nullptr;
}

FTPixelShader* ResourceManager::GetLoadedPixelShader(const UINT key)
{
	FTPixelShader* ps = mPixelShaders.At(key);
	if (!ps)
		Debug::LogError(__LINE__, __FILE__, "PixelShader is NULL");
	return ps;
}

FTMaterial* ResourceManager::GetLoadedMaterial(const UINT key)
{
	FTMaterial* mat = mMaterials.At(key);
	if (!mat)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is NULL");
	return mat;
}

FTBasicMeshGroup* ResourceManager::GetLoadedMesh(const UINT key)
{
	FTBasicMeshGroup* meshGrp = mMeshGroups.At(key);
	if (!meshGrp)
		Debug::LogError(__LINE__, __FILE__, "FTMeshGroup is NULL");
	return meshGrp;
}

FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(const UINT key)
{
	FTSpriteAnimation* spriteAnim = mSpriteAnimations.At(key);
	if (!spriteAnim)
		Debug::LogError(__LINE__, __FILE__, "FTSpirteAnimation is NULL");
	return spriteAnim;
}

FTCSV* ResourceManager::GetLoadedCSV(const UINT key)
{
	FTCSV* ftCSV = mCSVs.At(key);
	if (!ftCSV)
		Debug::LogError(__LINE__, __FILE__, "FTCSV is NULL");
	return ftCSV;
}

FTJSON* ResourceManager::GetLoadedJSON(const UINT key)
{
	FTJSON* ftJSON = mJSONs.At(key);
	if (!ftJSON)
		Debug::LogError(__LINE__, __FILE__, "FTJSON is NULL");
	return ftJSON;
}