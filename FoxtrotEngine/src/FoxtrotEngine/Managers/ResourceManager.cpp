#include <unordered_map>

#include "spine/Atlas.h"

#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/FTSpineAnimation.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"
#include "FoxtrotEngine/ResourceSystem/FTTileMap.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Renderer/D3D11Utils.h"

UINT ResourceManager::gItemID = 0;

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;
}

void ResourceManager::LoadTexture(const std::string key, const std::string filePath)
{
	FTTexture* ptTex = nullptr;
	if (!ResourceExists<FTTexture*>(key, filePath, mMapTextures)) {
		printf("Message: Loading FTTexture %s to key %s. \n", filePath.c_str(), key.c_str());
		ptTex = new FTTexture;
		ptTex->SetRelativePath(filePath);
		D3D11Utils::CreateTexture(mRenderer->GetDevice(), mRenderer->GetContext(), ptTex);
		if (!ptTex)
			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
		mMapTextures.insert(std::make_pair(key, ptTex));
	}
	else {
		printf("Warning : FTTexture %s is already loaded to key %s.\n", filePath.c_str(), key.c_str());
	}
}

//void ResourceManager::LoadSpineTexture(FoxtrotRenderer* renderer, spine::String fileName)
//{
//	FTTexture* ptTex = FindTexture(fileName.buffer());
//	if (ptTex != nullptr)
//		printf("Warning : FTSpineTexture %ls is already loaded.\n", fileName.buffer());
//	else {
//		printf("Message: Loading FTTexture %s\n", fileName.buffer());
//		FTSpineTexture* spineTex = new FTSpineTexture;
//		std::string fileNameStr = fileName.buffer();
//		spineTex->SetRelativePath(fileNameStr);
//		if (!spineTex->CreateTexture(renderer)) {
//			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
//			return;
//		}
//		load(*spineTex->GetAtlasPage(), spine::String(spineTex->GetRelativePath().c_str()));
//		mMapTextures.insert(std::make_pair(fileNameStr, ptTex));
//	}
//}

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

//bool ResourceManager::LoadSpineAnimation(std::string key, std::string atlasPath, std::string skeletonDataPath)
//{
//	FTSpineAnimation* spineAnim = new FTSpineAnimation;
//	if (!spineAnim->CreateAtlas(atlasPath, mSpineTextureLoader)) {
//		LogString("ResourceManager::LoadSpineAnimation()-> CreateAtlas() Failed.");
//		return false;
//	};
//	if (!spineAnim->CreateSkeletonDataBinary(skeletonDataPath)) {
//		LogString("ResourceManager::LoadSpineAnimation()-> CreateSkeletonData() Failed.");
//		return false;
//	}
//	if (!spineAnim->CreateAnimationStateData()) {
//		LogString("ResourceManager::LoadSpineAnimation()-> CreateAnimationStateData() Failed.");
//		return false;
//	}
//	mMapSpineAnimData.insert(std::make_pair(key, spineAnim));
//	return true;
//}

void ResourceManager::LoadMeshFromFile(const std::string key, const std::string filePath)
{
	std::string path = mPathToAsset + filePath;
	if (!KeyExists(key, mMapMeshes)) {
		printf("Message: Loading Mesh %s to key %s. \n", path.c_str(), key.c_str());
		std::vector<MeshData> meshData = GeometryGenerator::ReadFromFile(filePath);
		if (!meshData.empty())
		{
			printf("Error: ResourceManager::LoadMesh() -> LoadMesh failed. \n");
			return;
		}
		mMapMeshes.insert(std::make_pair(key, meshData));
	}
	else {
		printf("Warning : FTTexture %s is already loaded to key %s.\n", filePath.c_str(), key.c_str());
	}
}

void ResourceManager::LoadBasicMesh(const std::string key, MeshData meshData)
{
	std::vector<MeshData> meshDat;
	meshDat.push_back(meshData);
	if (meshDat.empty())
	{
		printf("Error: ResourceManager::LoadMesh() -> LoadSquareMesh failed. \n");
		return;
	}
	mMapMeshes.insert(std::make_pair(key, meshDat));
}

void ResourceManager::LoadBasicMesh(const std::string key, std::vector<MeshData> meshData)
{
	if (meshData.empty())
	{
		printf("Error: ResourceManager::LoadMesh() -> LoadSquareMesh failed. \n");
		return;
	}
	mMapMeshes.insert(std::make_pair(key, meshData));
}

void ResourceManager::LoadTileMap(const std::string key, const std::string filePath)
{
	std::string path = mPathToAsset + filePath;
	if (!KeyExists(key, mMapTileMaps)) {
		printf("Message: Loading tilemap %s to key %s. \n", path.c_str(), key.c_str());
		FTTileMap* tileMap = new FTTileMap;
		tileMap->SetRelativePath(path);
		mMapTileMaps.insert(std::make_pair(key, tileMap));
	}
	else {
		printf("Warning : FTTileMap %s is already loaded to key %s.\n", filePath.c_str(), key.c_str());
	}
}

FTTexture* ResourceManager::GetLoadedTexture(const std::string key)
{
	FTTexture* ptTex = mMapTextures.at(key);
	if (ptTex != nullptr)
		return ptTex;
	else
	{
		printf("Error: Unable to find FTTexture with key; %s\n", key.c_str());
		return nullptr;
	}
}

//FTSpineAnimation* ResourceManager::GetLoadedSpineAnimation(std::string key)
//{
//	FTSpineAnimation* ptAnim = FindSpineAnimation(key);
//	if (ptAnim != nullptr)
//		return ptAnim;
//	else
//	{
//		printf("Error: Unable to find FTTexture with keyName; %s\n", key.c_str());
//		return nullptr;
//	}
//}

//FTSpineAnimation* ResourceManager::FindSpineAnimation(const std::string keyName)
//{
//	auto iter = mMapSpineAnimData.find(keyName);
//	if (iter != mMapSpineAnimData.end())
//		return iter->second;
//	else
//	{
//		printf("Error: Cannot find SpineAnimation %s\n", keyName.c_str());
//		return nullptr;
//	}
//}

std::vector<MeshData>& ResourceManager::GetLoadedMeshes(const std::string key)
{
	std::vector<MeshData>& meshes = mMapMeshes.at(key);
	if (!meshes.empty())
		return meshes;
	else
		printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %s\n", key.c_str());
}

FTTileMap* ResourceManager::GetLoadedTileMap(const std::string key)
{
	FTTileMap* tileMap = mMapTileMaps.at(key);
	if (tileMap)
		return tileMap;
	else
		printf("Error: ResourceManager::GetLoadedTileMap() -> FTTileMap is empty %s\n", key.c_str());
}

void ResourceManager::RemoveLoadedMeshes(const std::string key)
{
	if (KeyExists(key, mMapMeshes)) {
		mMapMeshes.erase(key);
	}
	else {
		printf("Error: ResourceManager::RemoveLoadedMeshes() -> Mesh with key %s does not exist\n", key.c_str());
	}
}

//spine::SpineExtension* getDefaultExtension() {
//	return new spine::DefaultSpineExtension();
//}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::string, FTTexture*>::iterator iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
	: mSpineTextureLoader(new SpineTextureLoader)
	, mPathToAsset("./assets/")
{

}

#ifdef _DEBUG
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

void ResourceManager::SaveResources(nlohmann::ordered_json& out)
{
	std::unordered_map<std::string, FTTexture*>::const_iterator iter;
	for (iter = mMapTextures.begin(); iter != mMapTextures.end(); ++iter) {
		(*iter).second->SaveProperties(out["Textures"][(*iter).first]);
	}
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
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string extension = path.substr(path.rfind("."));
			
			if (StrContains(TEXTURE_FORMAT_SUPPORTED, extension)) {
				std::string relativePath = path.substr(path.rfind("assets"));
				ResourceManager::GetInstance()->LoadTexture("Insert Key", relativePath);
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}
	if (ImGui::TreeNode("Textures"))
	{
		std::unordered_map<std::string, FTTexture*>::const_iterator texIter;
		texIter = mMapTextures.begin();
		for (texIter = mMapTextures.begin(); texIter != mMapTextures.end(); ++texIter) {
			std::string updatedKey = (*texIter).first;
			(*texIter).second->UpdateUI(updatedKey);
			if ((*texIter).first.compare(updatedKey)) {
				
			}
		}
		ImGui::TreePop();
	}
}
#endif

void SpineTextureLoader::load(spine::AtlasPage& page, const spine::String& path)
{
	FTTexture* tex = ResourceManager::GetInstance()->GetLoadedTexture(path.buffer());
	if (tex == nullptr)
		LogString("SpineTextureLoader::Load() -> Failed to load texture");
	page.texture = tex;
}

void SpineTextureLoader::unload(void* texture)
{
}
