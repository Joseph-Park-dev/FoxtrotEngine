#include "EditorResourceManager.h"

#include "Compare/StringEqual.h"
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

#include "EditorChunkLoader.h"
#include "DirectoryHelper.h"

#include "Utils/StrAssign.h"

void EditorResourceManager::LoadAllResourcesInAsset()
{
	DirectoryHelper::IterateForFileRecurse(
		GetPathToAsset(),
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

void EditorResourceManager::LoadResByType(std::string& filePath)
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

void EditorResourceManager::LoadMaterials()
{
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

	mMapMaterials.insert({ FTDS::MakeHeapStr(ChunkKey::STANDARD_MAT), standard });
	mMapMaterials.insert({ FTDS::MakeHeapStr(ChunkKey::RIM_MAT), rim });
}

void EditorResourceManager::PassLoadResourceInChunk(std::ifstream& ifs)
{
	std::pair<size_t, std::string> resPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t						   packCount = resPack.first;

	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
}


std::unordered_map<const char*, FTTexture*>& EditorResourceManager::GetTexturesMap()
{
	return mMapTextures;
}

std::unordered_map<const char*, FTTileMap*>& EditorResourceManager::GetTileMapsMap()
{
	return mMapTileMaps;
}

std::unordered_map<const char*, FTSpriteSheet*>& EditorResourceManager::GetSpriteSheetsMap()
{
	return mMapSpriteSheets;
}

std::unordered_map<const char*, FTSpriteAnimation*>& EditorResourceManager::GetSpriteAnimMap()
{
	return mMapSpriteAnimation;
}

std::unordered_map<const char*, FTBasicMeshGroup*>& EditorResourceManager::GetMeshGroupsMap()
{
	return mMapMeshGroups;
}

std::unordered_map<const char*, FTVertexShader*>& EditorResourceManager::GetVertexShadersMap()
{
	return mMapVertexShaders;
}

std::unordered_map<const char*, FTPixelShader*>& EditorResourceManager::GetPixelShadersMap()
{
	return mMapPixelShaders;
}

std::unordered_map<const char*, FTMaterial*>& EditorResourceManager::GetMapMaterials()
{
	return mMapMaterials;
}

std::unordered_map<const char*, FTCSV*>& EditorResourceManager::GetMapCSVs()
{
	return mMapCSVs;
}

std::unordered_map<const char*, FTJSON*>& EditorResourceManager::GetMapJSONs()
{
	return mMapJSONs;
}

FTTexture* EditorResourceManager::GetLoadedTexture(const char* key)
{
	FTTexture* ptTex = mMapTextures.at(key);
	if (!ptTex)
		printf("Error: Unable to find FTTexture with key; %s\n", key);
	ptTex->AddRefCount();
	return ptTex;
}

FTTileMap* EditorResourceManager::GetLoadedTileMap(const char* key)
{
	FTTileMap* tileMap = mMapTileMaps.at(key);
	if (!tileMap)
	{
		printf("Error: EditorResourceManager::GetLoadedTileMap() -> FTTileMap is empty %s\n", key);
		return nullptr;
	}

	tileMap->AddRefCount();
	return tileMap;
}

FTSpriteSheet* EditorResourceManager::GetLoadedSpriteSheet(const char* key)
{
	if(mMapSpriteSheets.find(key) == mMapSpriteSheets.end())
		return nullptr;

	FTSpriteSheet* spriteSheet = mMapSpriteSheets.at(key);
	if (!spriteSheet)
	{
		printf("Error: EditorResourceManager::GetLoadedTileMap() -> FTSpriteSheet is empty %s\n", key);
		return nullptr;
	}
	spriteSheet->AddRefCount();
	return spriteSheet;
}

FTPremade* EditorResourceManager::GetLoadedPremade(const char* key)
{
	FTPremade* premade = mMapPremades.at(key);
	if (!premade)
		printf("Error: EditorResourceManager::GetLoadedPremade() -> FTPremade is empty %s\n", key);
	premade->AddRefCount();
	return premade;
}

FTPixelShader* EditorResourceManager::GetLoadedPixelShader(const char* key)
{
	FTPixelShader* shader = mMapPixelShaders.at(key);
	if (!shader)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
	shader->AddRefCount();
	return shader;
}

FTMaterial* EditorResourceManager::GetLoadedMaterial(const char* key)
{
	FTMaterial* material = mMapMaterials.at(key);
	if (!material)
		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
	material->AddRefCount();
	return material;
}

FTBasicMeshGroup* EditorResourceManager::GetLoadedMesh(const char* key)
{
	FTBasicMeshGroup* meshGrp = mMapMeshGroups.at(key);
	if (meshGrp)
	{
		mMapMeshGroups.at(key)->AddRefCount();
		return meshGrp;
	}
	printf("Error: EditorResourceManager::GetLoadedMeshes() -> Mesh is empty %s\n", key);
	return nullptr;
}

FTSpriteAnimation* EditorResourceManager::GetLoadedSpriteAnim(const char* key)
{
	FTSpriteAnimation* spriteAnim = mMapSpriteAnimation.at(key);
	if (spriteAnim)
	{
		spriteAnim->AddRefCount();
		return spriteAnim;
	}
	Debug::LogError(__LINE__, __FILE__, "Null animation");
	return nullptr;
}

FTCSV* EditorResourceManager::GetLoadedCSV(const char* key)
{
	FTCSV* csv = mMapCSVs.at(key);
	if (!csv)
		Debug::LogError(__LINE__, __FILE__, "Failed to load FTCSV");
	csv->AddRefCount();
	return csv;
}

FTJSON* EditorResourceManager::GetLoadedJSON(const char* key)
{
	FTJSON* json = mMapJSONs.at(key);
	if (!json)
		Debug::LogError(__LINE__, __FILE__, "Failed to load FTCSV");
	json->AddRefCount();
	return json;
}

void EditorResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	ResourceManager::Initialize(renderer);

	mMapTextures.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapTileMaps.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapSpriteSheets.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapPremades.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapSpriteAnimation.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapMeshGroups.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });

	mMapVertexShaders.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapPixelShaders.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapMaterials.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapCSVs.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });
	mMapJSONs.insert({ ChunkKey::NullVal::NULL_OBJECT, nullptr });

	// Add primitive geometries as resources
	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_RED),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(1.0f, 0.0f, 0.0f)), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_GREEN),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(0.0f, 1.0f, 0.0f)), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_BLUE),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquare(FTVector3(0.0f, 0.0f, 1.0f)), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_BOX),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeBox(), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_GRID),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSquareGrid(1.0f, 1.0f, 2, 2), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_CYLINDER),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeCylinder(1.0f, 1.0f, 2, 5), renderer) });

	mMapMeshGroups.insert(
		{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SPHERE),
		  DBG_NEW FTBasicMeshGroup(
			  GeometryGenerator::MakeSphere(1.0f, 50, 50), renderer) });
}

void EditorResourceManager::DeleteAll()
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

void EditorResourceManager::ProcessTextures()
{
	for (auto& textureItem : mMapTextures)
		if (textureItem.second)
			ProcessTexture(textureItem.second);
}

void EditorResourceManager::ProcessMeshGroups()
{
	for (auto& meshGrp : mMapMeshGroups)
		if (meshGrp.second)
			ProcessSingleMeshGrp(meshGrp.second);
}

void EditorResourceManager::ProcessPremades()
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

void EditorResourceManager::ProcessTileMaps()
{
	for (auto& tileMapItem : mMapTileMaps)
		if (tileMapItem.second)
			ProcessTileMap(tileMapItem.second);
}

void EditorResourceManager::ProcessSpriteSheets()
{
	for (auto& spriteSheetItem : mMapSpriteSheets)
		if (spriteSheetItem.second)
			ProcessSpriteSheet(spriteSheetItem.second);
}

void EditorResourceManager::ProcessSpriteAnims()
{
	for (auto& animMapItem : mMapSpriteAnimation)
		if (animMapItem.second)
			ProcessSpriteAnim(animMapItem.second);
}

void EditorResourceManager::ProcessCSVs()
{
	for (auto& csvItem : mMapCSVs)
		if (csvItem.second)
			ProcessCSV(csvItem.second);
}

void EditorResourceManager::ProcessJSONs()
{
	for (auto& jsonItem : mMapJSONs)
		if (jsonItem.second)
			ProcessJSON(jsonItem.second);
}

void EditorResourceManager::ProcessMaterials()
{
	for (auto& material : mMapMaterials)
		if (material.second)
			ProcessMaterial(material.second);
}

void EditorResourceManager::ProcessVertexShaders()
{
	for (auto& shader : mMapVertexShaders)
		if (shader.second)
			shader.second->CompileShader(GetRenderer());
}

void EditorResourceManager::ProcessPixelShaders()
{
	for (auto& shader : mMapPixelShaders)
		if (shader.second)
			shader.second->CompileShader(GetRenderer());
}

void EditorResourceManager::UpdateUI()
{
	if (ImGui::Button("Import"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
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
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string extension = path.substr(path.rfind("."));

			if (StrContains(FileTypes::TEXTURE, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTexture* texture = LoadResource<FTTexture>(relativePath, mMapTextures);
				ProcessTexture(texture);
			}
			else if (StrContains(FileTypes::TILEMAP, extension))
			{
				std::string relativePath = path.substr(path.rfind("Assets"));
				FTTileMap* tileMap = LoadResource<FTTileMap>(relativePath, mMapTileMaps);
			}
			else if (StrContains(FileTypes::PREMADE, extension))
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
		std::unordered_map<const char*, FTTexture*>::const_iterator texIter;
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
		std::unordered_map<const char*, FTTileMap*>::const_iterator tileIter;
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
		std::unordered_map<const char*, FTPremade*>::const_iterator premadeIter;
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
		std::unordered_map<const char*, FTVertexShader*>::const_iterator vsIter;
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
		std::unordered_map<const char*, FTMaterial*>::const_iterator materialIter;
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

ResType EditorResourceManager::GetResType(std::string& fileName)
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

EditorResourceManager::EditorResourceManager()
{ }

EditorResourceManager::~EditorResourceManager()
{
}