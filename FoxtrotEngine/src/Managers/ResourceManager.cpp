// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTSpriteAnimation.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/ModelLoader.h"
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

	// Add primitive geometries as resources
	mMap2DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_SQUARE_RED,
			GeometryGenerator::MakeSquare(FTVector3(1.0f, 0.0f, 0.0f))));
	mMap2DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_SQUARE_GREEN,
			GeometryGenerator::MakeSquare(FTVector3(0.0f, 1.0f, 0.0f))));
	mMap2DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_SQUARE_BLUE,
			GeometryGenerator::MakeSquare(FTVector3(0.0f, 0.0f, 1.0f))));

	mMap3DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_BOX,
			GeometryGenerator::MakeBox()));
	mMap3DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_SQUARE_GRID,
			GeometryGenerator::MakeSquareGrid(1.0f, 1.0f, 2, 2)));
	mMap3DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_CYLINDER,
			GeometryGenerator::MakeCylinder(1.0f, 1.0f, 2, 5)));
	mMap3DPrimitives.insert(
		std::pair(
			ChunkKey::PRIMITIVE_SPHERE,
			GeometryGenerator::MakeSphere(1.0f, 10, 10)));
}

void ResourceManager::DeleteAll()
{
	ClearMap<FTTexture>(mMapTextures);
	ClearMap<FTTileMap>(mMapTileMaps);
	ClearMap<FTPremade>(mMapPremades);
	ClearMap<FTSpriteAnimation>(mMapSpriteAnimation);
	ClearMap<FTMeshDataPack>(mMapMeshData);
	mMapMeshData.clear();
	mMap2DPrimitives.clear();
	mMap3DPrimitives.clear();
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
		if (FTDS::StringEqual((*iter).second->GetFileName().c_str(), name))
		{
			(*iter).second->AddRefCount();
			return (*iter).second;
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
	std::string									   premadeFullName = fileName + FileTypes::PREMADE;
	std::unordered_map<UINT, FTPremade*>::iterator iter			   = mMapPremades.begin();
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

FTMeshDataPack* ResourceManager::GetLoadedMeshData(const UINT key)
{
	FTMeshDataPack* meshes = mMapMeshData.at(key);
	if (!meshes)
		printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %d\n", key);
	return meshes;
}

FTMeshData& ResourceManager::GetLoaded2DPrimitive(const UINT key)
{
	FTMeshData& primitive = mMap2DPrimitives.at(key);
	if (primitive.IsEmpty())
		printf("Error: ResourceManager::GetLoadedPrimitive() -> Primitive is empty %d\n", key);
	return primitive;
}

FTMeshData& ResourceManager::GetLoaded3DPrimitive(const UINT key)
{
	FTMeshData& primitive = mMap3DPrimitives.at(key);
	if (primitive.IsEmpty())
		printf("Error: ResourceManager::GetLoadedPrimitive() -> Primitive is empty %d\n", key);
	return primitive;
}

void ResourceManager::RemoveLoadedMeshes(const UINT key)
{
	if (KeyExists(key, mMapMeshData))
	{
		delete mMapMeshData.at(key);
		mMapMeshData.erase(key);
	}
	else
	{
		printf("Error: ResourceManager::RemoveLoadedMeshes() -> Mesh with key %d does not exist\n", key);
	}
}

FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(const UINT key)
{
	FTSpriteAnimation* spriteAnim = mMapSpriteAnimation.at(key);
	if (!spriteAnim)
		printf("Error: ResourceManager::GetLoadedSpriteAnim() -> FTSpriteAnimation is empty %d\n", key);
	spriteAnim->AddRefCount();
	return spriteAnim;
}

std::unordered_map<UINT, FTTexture*>& ResourceManager::GetTexturesMap()
{
	return mMapTextures;
}

std::unordered_map<UINT, FTTileMap*>& ResourceManager::GetTileMapsMap()
{
	return mMapTileMaps;
}

std::unordered_map<UINT, FTSpriteAnimation*>& ResourceManager::GetSpriteAnimMap()
{
	return mMapSpriteAnimation;
}

std::unordered_map<UINT, FTMeshDataPack*>& ResourceManager::GetMeshDataMap()
{
	return mMapMeshData;
}

std::string& ResourceManager::GetPathToAsset()
{
	return mPathToAsset;
}

void ResourceManager::SetPathToAsset(std::string&& projectPath)
{
	mPathToAsset.assign(projectPath + "\\Assets");
}

void ResourceManager::ProcessTexture(FTTexture* texture)
{
	if (texture->GetIsProcessed())
		return;

	D3D11Utils::CreateTexture(mRenderer->GetDevice(), mRenderer->GetContext(), texture);

	if (!texture)
		printf("ERROR : ResourceManager::ProcessTexture()->CreateTexture() Failed");
	else
		texture->SetIsProcessed(true);
}

void ResourceManager::ProcessSingleMeshData(FTMeshDataPack* meshDataPack)
{
	meshDataPack->GetMeshData() = 
		GeometryGenerator::ReadFromFile(meshDataPack->GetRelativePath());
}

void ResourceManager::ProcessTileMap(FTTileMap* tileMap)
{
	/*if (tileMap)
		tileMap->ReadCSV();
	else
		printf("ERROR : ResourceManager::ProcessTexture()->TileMap is null");*/
}

void ResourceManager::ProcessSpriteAnim(FTSpriteAnimation* spriteAnim)
{
	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(spriteAnim->GetTileMapKey());
	if (tileMapBuf)
		tileMapBuf->ReadCSV();

	std::vector<FTMeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tileMapBuf->GetTiles(), tileMapBuf->GetMaxCountOnMapX(), tileMapBuf->GetMaxCountOnMapY());
	spriteAnim->Initialize(meshDataBuf, mRenderer->GetDevice(), mRenderer->GetContext());
}

void ResourceManager::ProcessTextures()
{
	for (auto& textureItem : mMapTextures)
		ProcessTexture(textureItem.second);
}

void ResourceManager::ProcessMeshData()
{
	for (auto& meshData : mMapMeshData)
		ProcessSingleMeshData(meshData.second);
}

void ResourceManager::ProcessPremades()
{
	for (auto& premadeItem : mMapPremades)
	{
		premadeItem.second->Load();
		// All loaded premades are included as default.
		premadeItem.second->AddRefCount();
	}
}

void ResourceManager::ProcessTileMaps()
{
	for (auto& tileMapItem : mMapTileMaps)
		ProcessTileMap(tileMapItem.second);
}

void ResourceManager::ProcessSpriteAnims()
{
	for (auto& animMapItem : mMapSpriteAnimation)
		ProcessSpriteAnim(animMapItem.second);
}

ResourceManager::~ResourceManager()
{
	DeleteAll();
}

ResourceManager::ResourceManager()
	: mItemKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mPathToAsset("./Assets")
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

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade>(ofs, mMapPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, mMapSpriteAnimation);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs, FTCore* ftCoreInst)
{
	std::pair<size_t, std::string> resPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t						   count   = resPack.first;

	std::pair<size_t, std::string> ftSpriteAnimPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	mMapSpriteAnimation.reserve(ftSpriteAnimPack.first);
	LoadResourceFromChunk<FTSpriteAnimation>(ifs, mMapSpriteAnimation, ftSpriteAnimPack.first);

	std::pair<size_t, std::string> ftPremadePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	mMapPremades.reserve(ftPremadePack.first);
	LoadResourceFromChunk<FTPremade>(ifs, mMapPremades, ftPremadePack.first);

	std::pair<size_t, std::string> ftTileMapPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	mMapTileMaps.reserve(ftTileMapPack.first);
	LoadResourceFromChunk<FTTileMap>(ifs, mMapTileMaps, ftTileMapPack.first);

	std::pair<size_t, std::string> ftTexturePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
	mMapTextures.reserve(ftTexturePack.first);
	LoadResourceFromChunk<FTTexture>(ifs, mMapTextures, ftTexturePack.first);

	ProcessTextures();
	ProcessMeshData();
	ProcessTileMaps();
	ProcessSpriteAnims();
	ProcessPremades();
}

#ifdef FOXTROT_EDITOR
void ResourceManager::LoadAllResourcesInAsset()
{
	mItemKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	DirectoryHelper::IterateForFileRecurse(
		mPathToAsset,
		[&](std::string&& path) { LoadResByType(path); });
	ProcessTextures();
	ProcessMeshData();
	ProcessTileMaps();
	ProcessSpriteAnims();
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
		case ResType::FTPREMADE:
			LoadResource(filePath, mMapPremades);
			break;
		case ResType::FTMESH:
			LoadResource(filePath, mMapMeshData);
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
	else if (StrContains(FileTypes::PREMADE, format))
		return ResType::FTPREMADE;
	else if (StrContains(FileTypes::MESH, format))
		return ResType::FTMESH;
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
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("TileMaps"))
	{
		std::unordered_map<UINT, FTTileMap*>::const_iterator tileIter;
		tileIter = mMapTileMaps.begin();
		for (; tileIter != mMapTileMaps.end(); ++tileIter)
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
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Premades"))
	{
		std::unordered_map<UINT, FTPremade*>::const_iterator premadeIter;
		premadeIter = mMapPremades.begin();
		for (; premadeIter != mMapPremades.end(); ++premadeIter)
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
		ImGui::TreePop();
	}
}
#endif // FOXTROT_EDITOR