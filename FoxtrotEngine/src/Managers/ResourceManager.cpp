#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

UINT ResourceManager::gItemKey = 0;

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	// Add primitive geometries as resources
	mMapPrimitives.insert(
		std::pair(ChunkKeys::PRIMITIVE_SQUARE, GeometryGenerator::MakeSquare())
	);
}

void ResourceManager::DeleteAll()
{
	ClearMap<FTTexture>(mMapTextures);
	mMapMeshes.clear();
	ClearMap<FTTileMap>(mMapTileMaps);
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

//void ResourceManager::LoadMeshFromFile(const std::string filePath)
//{
//	std::string path = mPathToAsset + filePath;
//	UINT key = gItemKey;
//	if (!KeyExists(key, mMapMeshes)) {
//		printf("Message: Loading Mesh %s to key %d. \n", path.c_str(), key);
//		std::vector<MeshData> meshData = GeometryGenerator::ReadFromFile(filePath);
//		if (!meshData.empty())
//		{
//			printf("Error: ResourceManager::LoadMesh() -> LoadMesh failed. \n");
//			return;
//		}
//		mMapMeshes.insert(std::make_pair(key, meshData));
//		++gItemKey;
//	}
//	else {
//		printf("Warning : FTTexture %s is already loaded to key %d.\n", filePath.c_str(), key);
//	}
//}

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
		FTTileMap* tileMap = new FTTileMap;
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
		return ptTex;
	else
	{
		printf("Error: Unable to find FTTexture with key; %d\n", key);
		return nullptr;
	}
}

std::vector<MeshData>& ResourceManager::GetLoadedMeshes(UINT key)
{
	std::vector<MeshData>& meshes = mMapMeshes.at(key);
	if (!meshes.empty())
		return meshes;
	else
		printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %d\n", key);
}

FTTileMap* ResourceManager::GetLoadedTileMap(UINT key)
{
	FTTileMap* tileMap = mMapTileMaps.at(key);
	if (tileMap)
		return tileMap;
	else
		printf("Error: ResourceManager::GetLoadedTileMap() -> FTTileMap is empty %d\n", key);
}

MeshData& ResourceManager::GetLoadedPrimitive(UINT key)
{
	MeshData& primitive = mMapPrimitives.at(key);
	if (!primitive.IsEmpty())
		return primitive;
	else
		printf("Error: ResourceManager::GetLoadedPrimitive() -> Primitive is empty %d\n", key);
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
	for (auto& textureItem : mMapTextures) {
		ProcessTexture(textureItem.second);
	}
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<UINT, FTTexture*>::iterator iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); iter++)
	{
		delete iter->second;
	}
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

	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs)
{
	std::pair<size_t, std::string>&& resPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::RESOURCE_DATA);
	size_t count = resPack.first;

	std::pair<size_t, std::string>&& ftTileMapPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTILEMAP_GROUP);
	LoadResourceToMap<FTTileMap>(ifs, mMapTileMaps, ftTileMapPack.first);

	std::pair<size_t, std::string>&& ftTexturePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTEXTURE_GROUP);
	LoadResourceToMap<FTTexture>(ifs, mMapTextures, ftTexturePack.first);
	ProcessTextures();
}

#ifdef FOXTROT_EDITOR
#include <nlohmann/json.hpp>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

void ResourceManager::SaveResources(nlohmann::ordered_json& out)
{
	SaveResourceInMap<FTTexture>(out, mMapTextures);
	//SaveResourceInMap<FTTileMap>(out, mMapTextures);
}

void ResourceManager::LoadResources(nlohmann::ordered_json& resourceTree)
{
	LoadResourceToMap<FTTexture>(resourceTree, mMapTextures);
	ProcessTextures();
}

void ResourceManager::UpdateUI()
{
	if (ImGui::Button("Import")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		std::string supportedFormat = TEXTURE_FORMAT_SUPPORTED + std::string(", .csv");
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
				std::string relativePath = path.substr(path.rfind("assets"));
				FTTexture* texture = LoadResource<FTTexture>(relativePath, mMapTextures);
				ProcessTexture(texture);
			}
			else if (StrContains(TILEMAP_FORMAT_SUPPORTED, extension)) 
			{
				std::string relativePath = path.substr(path.rfind("assets"));
				FTTileMap* tileMap = LoadResource<FTTileMap>(relativePath, mMapTileMaps);
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
}
#endif // FOXTROT_EDITOR