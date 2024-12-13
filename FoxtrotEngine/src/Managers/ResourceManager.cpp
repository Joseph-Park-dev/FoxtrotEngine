#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTSpriteAnimation.h"
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
	ClearMap<FTSpriteAnimation>(mMapSpriteAnimation);
	mMapMeshes.clear();
	mMapPrimitives.clear();
}

FTTexture* ResourceManager::GetLoadedTexture(const UINT key)
{
	FTTexture* ptTex = mMapTextures.at(key);
	if (!ptTex)
		printf("Error: Unable to find FTTexture with key; %d\n", key);
	ptTex->AddRefCount();
	return ptTex;
}

std::vector<MeshData>& ResourceManager::GetLoadedMeshes(const UINT key)
{
	std::vector<MeshData>& meshes = mMapMeshes.at(key);
	if (meshes.empty())
		printf("Error: ResourceManager::GetLoadedMeshes() -> Mesh is empty %d\n", key);
	return meshes;
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
	std::string premadeFullName = fileName + ChunkKeys::PREMADE_FILE_FORMAT;
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
MeshData& ResourceManager::GetLoadedPrimitive(const UINT key)
{
	MeshData& primitive = mMapPrimitives.at(key);
	if (primitive.IsEmpty())
		printf("Error: ResourceManager::GetLoadedPrimitive() -> Primitive is empty %d\n", key);
	return primitive;
}

void ResourceManager::RemoveLoadedMeshes(const UINT key)
{
	if (KeyExists(key, mMapMeshes)) {
		mMapMeshes.erase(key);
	}
	else {
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
	D3D11Utils::CreateTexture(mRenderer->GetDevice(), mRenderer->GetContext(), texture);
	if (!texture)
		printf("ERROR : ResourceManager::ProcessTexture()->CreateTexture() Failed");
}

void ResourceManager::ProcessTileMap(FTTileMap* tileMap)
{
	if (tileMap)
		tileMap->ReadCSV();
	else
		printf("ERROR : ResourceManager::ProcessTexture()->TileMap is null");
}

void ResourceManager::ProcessSpriteAnim(FTSpriteAnimation* spriteAnim)
{
	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(spriteAnim->GetTileMapKey());
	if (tileMapBuf)
		tileMapBuf->ReadCSV();

	if (spriteAnim->GetTexKey() != VALUE_NOT_ASSIGNED)
		spriteAnim->SetTexture(spriteAnim->GetTexKey());

	std::vector<MeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tileMapBuf->GetTiles(),
		tileMapBuf->GetMaxCountOnMapX(),
		tileMapBuf->GetMaxCountOnMapY()
	);
	spriteAnim->Initialize(meshDataBuf, mRenderer->GetDevice(), mRenderer->GetContext());
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
	: mPathToAsset("./Assets")
	, mRenderer(nullptr)
{}

void ResourceManager::SaveResources(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::RESOURCE_DATA);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTEXTURE_GROUP);
	SaveResourceToChunk<FTTexture>(ofs, mMapTextures);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTEXTURE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTILEMAP_GROUP);
	SaveResourceToChunk<FTTileMap>(ofs, mMapTileMaps);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTILEMAP_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTPREMADE_GROUP);
	SaveResourceToChunk<FTPremade>(ofs, mMapPremades);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTPREMADE_GROUP);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FT_SPRITE_ANIMATION_GROUP);
	SaveResourceToChunk<FTSpriteAnimation>(ofs, mMapSpriteAnimation);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FT_SPRITE_ANIMATION_GROUP);

	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::RESOURCE_DATA);
}

void ResourceManager::LoadResources(std::ifstream& ifs, FTCore* ftCoreInst)
{
	std::pair<size_t, std::string> resPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::RESOURCE_DATA);
	size_t count = resPack.first;

	std::pair<size_t, std::string> ftSpriteAnimPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FT_SPRITE_ANIMATION_GROUP);
	mMapTextures.reserve(ftSpriteAnimPack.first);
	LoadResourceFromChunk<FTSpriteAnimation>(ifs, mMapSpriteAnimation, ftSpriteAnimPack.first);

	std::pair<size_t, std::string> ftPremadePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTPREMADE_GROUP);
	mMapPremades.reserve(ftPremadePack.first);
	LoadResourceFromChunk<FTPremade>(ifs, mMapPremades, ftPremadePack.first);

	std::pair<size_t, std::string> ftTileMapPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTILEMAP_GROUP);
	mMapTileMaps.reserve(ftTileMapPack.first);
	LoadResourceFromChunk<FTTileMap>(ifs, mMapTileMaps, ftTileMapPack.first);

	std::pair<size_t, std::string> ftTexturePack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTEXTURE_GROUP);
	mMapTextures.reserve(ftTexturePack.first);
	LoadResourceFromChunk<FTTexture>(ifs, mMapTextures, ftTexturePack.first);
	
	ProcessTextures();
	ProcessTileMaps();
	ProcessSpriteAnims();
	ProcessPremades();
}

#ifdef FOXTROT_EDITOR
void ResourceManager::LoadAllResourcesInAsset()
{
	gItemKey = 0;
	DirectoryHelper::IterateForFileRecurse(
		mPathToAsset,
		[&](std::string&& path) { LoadResByType(path); }
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