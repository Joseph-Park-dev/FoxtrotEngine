#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTPremade.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

#include "DirectoryHelper.h"
#endif // FOXTROT_EDITOR

UINT ResourceManager::gItemKey = 0;

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	// Add primitive geometries as resources
	mMapPrimitives.insert(
		std::pair(
			ChunkKeys::PRIMITIVE_SQUARE,
			GeometryGenerator::MakeSquare()
		)
	);
}

void ResourceManager::DeleteAll()
{
	ClearMap<FTTexture>(mMapTextures);
	ClearMap<FTTileMap>(mMapTileMaps);
	ClearMap<FTPremade>(mMapPremades);

	mMapMeshes.clear();
	mMapPrimitives.clear();
}

void ResourceManager::UpdateTexture(FTTexture* texture, int channels)
{
	if (texture != nullptr)
	{
		if (!texture->ReleaseTexture())
		{
			LogString("Error: ResourceManager::UpdateTexture() : ReleaseTexture failed");
			return;
		}
		printf("Message: Loading FTTexture %s\n", texture->GetRelativePath().c_str());
		//if (!texture->CreateTexture(renderer))
		//	printf("Error: ResourceManager::LoadTexture() -> UpdateTexture failed. \n");
	}
	else
		printf("Warning : FTTexture %s is not loaded. Aborting update...\n", texture->GetRelativePath().c_str());
}

void ResourceManager::LoadBasicMesh(MeshData meshData)
{
	std::vector<MeshData> meshDat;
	meshDat.push_back(meshData);
	UINT key = gItemKey;
	if (meshDat.empty())
	{
		printf("Error: ResourceManager::LoadMesh() -> LoadSquareMesh failed. \n");
		return;
	}
	mMapMeshes.insert(std::make_pair(key, meshDat));
	++gItemKey;
}

UINT ResourceManager::LoadBasicMesh(std::vector<MeshData> meshData)
{
	UINT key = gItemKey;
	if (meshData.empty())
	{
		printf("Error: ResourceManager::LoadMesh() -> LoadSquareMesh failed. \n");
		return -1;
	}
	mMapMeshes.insert(std::make_pair(key, meshData));
	++gItemKey;
	return key;
}

void ResourceManager::LoadTileMap(const std::string filePath)
{
	std::string path = mPathToAsset + filePath;
	UINT key = gItemKey;
	if (!KeyExists(key, mMapTileMaps)) {
		printf("Message: Loading tilemap %s to key %d. \n", path.c_str(), key);
		FTTileMap* tileMap = DBG_NEW FTTileMap;
		tileMap->SetRelativePath(path);
		mMapTileMaps.insert(std::make_pair(key, tileMap));
		++gItemKey;
	}
	else {
		printf("Warning : FTTileMap %s is already loaded to key %d.\n", filePath.c_str(), key);
	}
}

FTTexture* ResourceManager::GetLoadedTexture(const UINT key)
{
	FTTexture* ptTex = mMapTextures.at(key);
	if (ptTex != nullptr)
		ptTex->SetIsReferenced(true);
	else
		printf("Error: Unable to find FTTexture with key; %d\n", key);
	return ptTex;
}

std::vector<MeshData>& ResourceManager::GetLoadedMeshes(UINT key)
{
	std::vector<MeshData>& meshes = mMapMeshes.at(key);
	if (meshes.empty())
		printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %d\n", key);
	return meshes;
}

FTTileMap* ResourceManager::GetLoadedTileMap(UINT key)
{
	FTTileMap* tileMap = mMapTileMaps.at(key);
	if (tileMap)
		tileMap->SetIsReferenced(true);
	else
		printf("Error: ResourceManager::GetLoadedTileMap() -> FTTileMap is empty %d\n", key);
	return tileMap;
}

FTPremade* ResourceManager::GetLoadedPremade(UINT key)
{
	FTPremade* premade = mMapPremades.at(key);
	if (premade)
		premade->SetIsReferenced(true);
	else
		printf("Error: ResourceManager::GetLoadedPremade() -> FTPremade is empty %d\n", key);
	return premade;
}

FTPremade* ResourceManager::GetLoadedPremade(std::string&& fileName)
{
	std::string premadeFullName = fileName + ChunkKeys::PREMADE_FILE_FORMAT;
	std::unordered_map<UINT, FTPremade*>::iterator iter = mMapPremades.begin();
	for (; iter != mMapPremades.end(); ++iter)
	{
		if ((*iter).second->GetFileName() == premadeFullName)
		{
			(*iter).second->SetIsReferenced(true);
			return (*iter).second;
		}
	}
	printf("Error: ResourceManager::GetLoadedPremade() -> Cannot find FTPremade %s\n", premadeFullName.c_str());
	return nullptr;
}

MeshData& ResourceManager::GetLoadedPrimitive(UINT key)
{
	MeshData& primitive = mMapPrimitives.at(key);
	if (primitive.IsEmpty())
		printf("Error: ResourceManager::GetLoadedPrimitive() -> Primitive is empty %d\n", key);
	return primitive;
}

void ResourceManager::RemoveLoadedMeshes(UINT key)
{
	if (KeyExists(key, mMapMeshes)) {
		mMapMeshes.erase(key);
	}
	else {
		printf("Error: ResourceManager::RemoveLoadedMeshes() -> Mesh with key %d does not exist\n", key);
	}
}

void ResourceManager::ProcessTexture(FTTexture* texture)
{
	D3D11Utils::CreateTexture(mRenderer->GetDevice(), mRenderer->GetContext(), texture);
	if (!texture)
		printf("ERROR : ResourceManager::ProcessTexture()->CreateTexture() Failed");
}

void ResourceManager::ProcessTextures()
{
	for (auto& textureItem : mMapTextures)
		ProcessTexture(textureItem.second);
}

void ResourceManager::ProcessPremades()
{
	for (auto& premadeItem : mMapPremades)
		premadeItem.second->Load();
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<UINT, FTTexture*>::iterator iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); iter++)
		delete iter->second;
}

ResourceManager::ResourceManager()
	: mPathToAsset("./Assets")
	, mRenderer(nullptr)
{

}

void ResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::RESOURCE_DATA);\

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTEXTURE_GROUP);
	SaveResourceInMap<FTTexture>(ofs, mMapTextures);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTILEMAP_GROUP);
	SaveResourceInMap<FTTileMap>(ofs, mMapTileMaps);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTPREMADE_GROUP);
	SaveResourceInMap<FTPremade>(ofs, mMapPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTPREMADE_GROUP);

	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs, FTCore* ftCoreInst)
{
	std::pair<size_t, std::string>&& resPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::RESOURCE_DATA);
	size_t count = resPack.first;

	std::pair<size_t, std::string>&& ftPremadePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTPREMADE_GROUP);
	LoadResourceToMap<FTPremade>(ifs, mMapPremades, ftPremadePack.first);

	std::pair<size_t, std::string>&& ftTileMapPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTILEMAP_GROUP);
	LoadResourceToMap<FTTileMap>(ifs, mMapTileMaps, ftTileMapPack.first);

	std::pair<size_t, std::string>&& ftTexturePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTEXTURE_GROUP);
	LoadResourceToMap<FTTexture>(ifs, mMapTextures, ftTexturePack.first);
	
	ProcessTextures();
	ProcessPremades();
}

#ifdef FOXTROT_EDITOR
void ResourceManager::LoadAllResourcesInAsset()
{
	DirectoryHelper::IterateForFileRecurse(
		mPathToAsset,
		[](std::string&& path) { ResourceManager::GetInstance()->LoadResByType(path); }
		);
	ProcessTextures();
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
	default:
		break;
	}
}

ResType ResourceManager::GetResType(std::string& fileName)
{
	std::string format = fileName.substr(fileName.rfind("."));
	if (StrContains(TEXTURE_FORMAT_SUPPORTED, format))
		return ResType::FTTEXTURE;
	else if (StrContains(TILEMAP_FORMAT_SUPPORTED, format))
		return ResType::FTTILEMAP;
	else if (StrContains(FTPremade_FORMAT_SUPPORTED, format))
		return ResType::FTPREMADE;
	else
		return ResType::UNSUPPORTED;
}

void ResourceManager::UpdateUI()
{
	if (ImGui::Button("Import")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;

		std::string supportedFormat =
			TEXTURE_FORMAT_SUPPORTED +
			std::string(", ") +
			TILEMAP_FORMAT_SUPPORTED +
			std::string(", ") +
			FTPremade_FORMAT_SUPPORTED;

		ImGuiFileDialog::Instance()->OpenDialog("SelectFile", "Select File", supportedFormat.c_str(), config);
		ImGui::OpenPopup("Select File");
	}
	if (ImGuiFileDialog::Instance()->Display("SelectFile"))
	{
		if (ImGuiFileDialog::Instance()->IsOk()) 
		{
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string extension = path.substr(path.rfind("."));
			
			if (StrContains(TEXTURE_FORMAT_SUPPORTED, extension)) 
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTexture* texture = LoadResource<FTTexture>(relativePath, mMapTextures);
				ProcessTexture(texture);
			}
			else if (StrContains(TILEMAP_FORMAT_SUPPORTED, extension)) 
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTileMap* tileMap = LoadResource<FTTileMap>(relativePath, mMapTileMaps);
			}
			else if (StrContains(FTPremade_FORMAT_SUPPORTED, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTPremade* premade = LoadResource<FTPremade>(relativePath, mMapPremades);
				premade->Load();
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::TreeNode("Textures"))
	{
		std::unordered_map<UINT, FTTexture*>::const_iterator texIter;
		texIter = mMapTextures.begin();
		for (texIter = mMapTextures.begin(); texIter != mMapTextures.end(); ++texIter) {
			if (ImGui::BeginListBox((*texIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 200)))
			{
				(*texIter).second->UpdateUI();
				if (ImGui::Button("Remove")) {
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
		for (; tileIter != mMapTileMaps.end(); ++tileIter) {
			if (ImGui::BeginListBox((*tileIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 200)))
			{
				(*tileIter).second->UpdateUI();
				if (ImGui::Button("Remove")) {
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
		for (; premadeIter != mMapPremades.end(); ++premadeIter) {
			if (ImGui::BeginListBox((*premadeIter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 100)))
			{
				(*premadeIter).second->UpdateUI();
				if (ImGui::Button("Remove")) {
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