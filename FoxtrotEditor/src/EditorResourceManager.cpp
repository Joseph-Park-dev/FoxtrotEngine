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

#include "Utils/StrAssign.h"
#include "Static/FTString.h"

void EditorResourceManager::LoadAllResourcesInAsset()
{
	ResourceManager::GetTextures()->Reserve(10);
	ResourceManager::GetTileMaps()->Reserve(10);
	ResourceManager::GetSpriteSheets()->Reserve(10);
	ResourceManager::GetPremades()->Reserve(10);
	ResourceManager::GetSpriteAnimations()->Reserve(10);
	ResourceManager::GetMeshGroups()->Reserve(10);
	ResourceManager::GetVertexShaders()->Reserve(10);
	ResourceManager::GetPixelShaders()->Reserve(10);
	ResourceManager::GetMaterials()->Reserve(10);
	ResourceManager::GetSounds()->Reserve(10);
	ResourceManager::GetCSVs()->Reserve(10);
	ResourceManager::GetJSONs()->Reserve(10);

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
	ProcessResources(FTCoreEditor::GetInstance(), GetMaterials());
	ProcessResources(FTCoreEditor::GetInstance(), GetVertexShaders());
	ProcessResources(FTCoreEditor::GetInstance(), GetPixelShaders());

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
		case ResType::FTCSV:
			LoadResource(path, GetCSVs());
			break;
		case ResType::FTJSON:
			LoadResource(path, GetJSONs());
			break;
		case ResType::FT_VERTEX_SHADER:
			LoadResource(path, GetVertexShaders());
			break;
		case ResType::FT_PIXEL_SHADER:
			LoadResource(path, GetPixelShaders());
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

	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SOUND);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMESH_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_SPRITE_ANIMATION_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSPRITESHEET_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTILEMAP_GROUP);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE_GROUP);
}

// std::unordered_map<FTDS::String, FTTexture*>& EditorResourceManager::GetTexturesMap()
//{
//	return mMapTextures;
// }
//
// std::unordered_map<FTDS::String, FTTileMap*>& EditorResourceManager::GetTileMapsMap()
//{
//	return mMapTileMaps;
// }
//
// std::unordered_map<FTDS::String, FTSpriteSheet*>& EditorResourceManager::GetSpriteSheetsMap()
//{
//	return mMapSpriteSheets;
// }
//
// std::unordered_map<FTDS::String, FTSpriteAnimation*>& EditorResourceManager::GetSpriteAnimMap()
//{
//	return mMapSpriteAnimation;
// }
//
// std::unordered_map<FTDS::String, FTBasicMeshGroup*>& EditorResourceManager::GetMeshGroupsMap()
//{
//	return mMapMeshGroups;
// }
//
// std::unordered_map<FTDS::String, FTVertexShader*>& EditorResourceManager::GetVertexShadersMap()
//{
//	return mMapVertexShaders;
// }
//
// std::unordered_map<FTDS::String, FTPixelShader*>& EditorResourceManager::GetPixelShadersMap()
//{
//	return mMapPixelShaders;
// }
//
// std::unordered_map<FTDS::String, FTMaterial*>& EditorResourceManager::GetMapMaterials()
//{
//	return mMapMaterials;
// }
//
// std::unordered_map<FTDS::String, FTCSV*>& EditorResourceManager::GetMapCSVs()
//{
//	return mMapCSVs;
// }
//
// std::unordered_map<FTDS::String, FTJSON*>& EditorResourceManager::GetMapJSONs()
//{
//	return mMapJSONs;
// }
//
// FTTexture* EditorResourceManager::GetLoadedTexture(FTDS::String key)
//{
//	FTTexture* ptTex = mMapTextures.at(key);
//	if (!ptTex)
//		printf("Error: Unable to find FTTexture with key; %s\n", key);
//	ptTex->AddRefCount();
//	return ptTex;
// }
//
// FTTileMap* EditorResourceManager::GetLoadedTileMap(FTDS::String key)
//{
//	FTTileMap* tileMap = mMapTileMaps.at(key);
//	if (!tileMap)
//	{
//		printf("Error: EditorResourceManager::GetLoadedTileMap() -> FTTileMap is empty %s\n", key);
//		return nullptr;
//	}
//
//	tileMap->AddRefCount();
//	return tileMap;
// }
//
// FTSpriteSheet* EditorResourceManager::GetLoadedSpriteSheet(FTDS::String key)
//{
//	if(mMapSpriteSheets.find(key) == mMapSpriteSheets.end())
//		return nullptr;
//
//	FTSpriteSheet* spriteSheet = mMapSpriteSheets.at(key);
//	if (!spriteSheet)
//	{
//		printf("Error: EditorResourceManager::GetLoadedTileMap() -> FTSpriteSheet is empty %s\n", key);
//		return nullptr;
//	}
//	spriteSheet->AddRefCount();
//	return spriteSheet;
// }
//
// FTPremade* EditorResourceManager::GetLoadedPremade(FTDS::String key)
//{
//	FTPremade* premade = mMapPremades.at(key);
//	if (!premade)
//		printf("Error: EditorResourceManager::GetLoadedPremade() -> FTPremade is empty %s\n", key);
//	premade->AddRefCount();
//	return premade;
// }
//
// FTPixelShader* EditorResourceManager::GetLoadedPixelShader(FTDS::String key)
//{
//	FTPixelShader* shader = mMapPixelShaders.at(key);
//	if (!shader)
//		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
//	shader->AddRefCount();
//	return shader;
// }
//
// FTMaterial* EditorResourceManager::GetLoadedMaterial(FTDS::String key)
//{
//	FTMaterial* material = mMapMaterials.at(key);
//	if (!material)
//		Debug::LogError(__LINE__, __FILE__, "FTMaterial is empty");
//	material->AddRefCount();
//	return material;
// }
//
// FTBasicMeshGroup* EditorResourceManager::GetLoadedMesh(FTDS::String key)
//{
//	FTBasicMeshGroup* meshGrp = mMapMeshGroups.at(key);
//	if (meshGrp)
//	{
//		mMapMeshGroups.at(key)->AddRefCount();
//		return meshGrp;
//	}
//	printf("Error: EditorResourceManager::GetLoadedMeshes() -> Mesh is empty %s\n", key);
//	return nullptr;
// }
//
// FTSpriteAnimation* EditorResourceManager::GetLoadedSpriteAnim(FTDS::String key)
//{
//	FTSpriteAnimation* spriteAnim = mMapSpriteAnimation.at(key);
//	if (spriteAnim)
//	{
//		spriteAnim->AddRefCount();
//		return spriteAnim;
//	}
//	Debug::LogError(__LINE__, __FILE__, "Null animation");
//	return nullptr;
// }
//
// FTCSV* EditorResourceManager::GetLoadedCSV(FTDS::String key)
//{
//	FTCSV* csv = mMapCSVs.at(key);
//	if (!csv)
//		Debug::LogError(__LINE__, __FILE__, "Failed to load FTCSV");
//	csv->AddRefCount();
//	return csv;
// }
//
// FTJSON* EditorResourceManager::GetLoadedJSON(FTDS::String key)
//{
//	FTJSON* json = mMapJSONs.at(key);
//	if (!json)
//		Debug::LogError(__LINE__, __FILE__, "Failed to load FTJSON");
//	json->AddRefCount();
//	return json;
// }

void EditorResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	ResourceManager::Initialize(renderer);

	//// Add primitive geometries as resources
	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_RED),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeSquare(FTVector3(1.0f, 0.0f, 0.0f)), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_GREEN),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeSquare(FTVector3(0.0f, 1.0f, 0.0f)), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_BLUE),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeSquare(FTVector3(0.0f, 0.0f, 1.0f)), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_BOX),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeBox(), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SQUARE_GRID),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeSquareGrid(1.0f, 1.0f, 2, 2), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_CYLINDER),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeCylinder(1.0f, 1.0f, 2, 5), renderer) });

	// mMapMeshGroups.insert(
	//	{ FTDS::MakeHeapStr(ChunkKey::PRIMITIVE_SPHERE),
	//	  DBG_NEW FTBasicMeshGroup(
	//		  GeometryGenerator::MakeSphere(1.0f, 50, 50), renderer) });
}

// void EditorResourceManager::DeleteAll()
//{
//	ClearMap<FTTexture>(mMapTextures);
//	ClearMap<FTTileMap>(mMapTileMaps);
//	ClearMap<FTSpriteSheet>(mMapSpriteSheets);
//	ClearMap<FTPremade>(mMapPremades);
//	ClearMap<FTSpriteAnimation>(mMapSpriteAnimation);
//	ClearMap<FTBasicMeshGroup>(mMapMeshGroups);
//	ClearMap<FTMaterial>(mMapMaterials);
//	ClearMap<FTCSV>(mMapCSVs);
//	ClearMap<FTJSON>(mMapJSONs);
// }

// void EditorResourceManager::ProcessTextures()
//{
//	for (auto& textureItem : mMapTextures)
//		if (textureItem.second)
//			ProcessTexture(textureItem.second);
// }
//
// void EditorResourceManager::ProcessMeshGroups()
//{
//	for (auto& meshGrp : mMapMeshGroups)
//		if (meshGrp.second)
//			ProcessSingleMeshGrp(meshGrp.second);
// }
//
// void EditorResourceManager::ProcessPremades()
//{
//	for (auto& premadeItem : mMapPremades)
//	{
//		if (premadeItem.second)
//		{
//			premadeItem.second->Load();
//			// All loaded premades are included as default.
//			premadeItem.second->AddRefCount();
//		}
//	}
// }
//
// void EditorResourceManager::ProcessTileMaps()
//{
//	for (auto& tileMapItem : mMapTileMaps)
//		if (tileMapItem.second)
//			ProcessTileMap(tileMapItem.second);
// }
//
// void EditorResourceManager::ProcessSpriteSheets()
//{
//	for (auto& spriteSheetItem : mMapSpriteSheets)
//		if (spriteSheetItem.second)
//			ProcessSpriteSheet(spriteSheetItem.second);
// }
//
// void EditorResourceManager::ProcessSpriteAnims()
//{
//	for (auto& animMapItem : mMapSpriteAnimation)
//		if (animMapItem.second)
//			ProcessSpriteAnim(animMapItem.second);
// }
//
// void EditorResourceManager::ProcessCSVs()
//{
//	for (auto& csvItem : mMapCSVs)
//		if (csvItem.second)
//			ProcessCSV(csvItem.second);
// }
//
// void EditorResourceManager::ProcessJSONs()
//{
//	for (auto& jsonItem : mMapJSONs)
//		if (jsonItem.second)
//			ProcessJSON(jsonItem.second);
// }
//
// void EditorResourceManager::ProcessMaterials()
//{
//	for (auto& material : mMapMaterials)
//		if (material.second)
//			ProcessMaterial(material.second);
// }
//
// void EditorResourceManager::ProcessVertexShaders()
//{
//	for (auto& shader : mMapVertexShaders)
//		if (shader.second)
//			shader.second->CompileShader(GetRenderer());
// }
//
// void EditorResourceManager::ProcessPixelShaders()
//{
//	for (auto& shader : mMapPixelShaders)
//		if (shader.second)
//			shader.second->CompileShader(GetRenderer());
// }

void EditorResourceManager::UpdateUI()
{
	/*if (ImGui::Button("Import"))
	{
		IGFD::FileDialogConfig config;
		config.path				 = ".";
		config.countSelectionMax = 1;

		FTDS::String supportedFormat =
			FileTypes::TEXTURE + FTDS::String(", ") + FileTypes::TILEMAP + FTDS::String(", ") + FileTypes::PREMADE;

		ImGuiFileDialog::Instance()->OpenDialog("SelectFile", "Select File", supportedFormat.c_str(), config);
		ImGui::OpenPopup("Select File");
	}
	if (ImGuiFileDialog::Instance()->Display("SelectFile"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			FTDS::String path	   = ImGuiFileDialog::Instance()->GetFilePathName().c_str();
			size_t		 pos	   = path.RFind(".");
			path.SubStr(pos, path.Length() - pos);

			if (StrContains(FileTypes::TEXTURE, path))
			{
				FTDS::String relativePath;
				path.SubStr(path.RFind("Assets"));
				FTTexture*	 texture	  = LoadResource<FTTexture>(relativePath, mMapTextures);
				ProcessTexture(texture);
			}
			else if (StrContains(FileTypes::TILEMAP, extension))
			{
				FTDS::String relativePath = path.substr(path.rfind("Assets"));
				FTTileMap*	 tileMap	  = LoadResource<FTTileMap>(relativePath, mMapTileMaps);
			}
			else if (StrContains(FileTypes::PREMADE, extension))
			{
				FTDS::String relativePath = path.substr(path.rfind("Assets"));
				FTPremade*	 premade	  = LoadResource<FTPremade>(relativePath, mMapPremades);
				premade->Load();
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::TreeNode("Textures"))
	{
		std::unordered_map<FTDS::String, FTTexture*>::const_iterator texIter;
		texIter = mMapTextures.begin();
		for (texIter = mMapTextures.begin(); texIter != mMapTextures.end(); ++texIter)
		{
			if ((*texIter).second)
			{
				if (ImGui::BeginListBox((*texIter).second->FileName().c_str(), ImVec2(-FLT_MIN, 200)))
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
		std::unordered_map<FTDS::String, FTTileMap*>::const_iterator tileIter;
		tileIter = mMapTileMaps.begin();
		for (; tileIter != mMapTileMaps.end(); ++tileIter)
		{
			if ((*tileIter).second)
			{
				if (ImGui::BeginListBox((*tileIter).second->FileName().c_str(), ImVec2(-FLT_MIN, 200)))
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
		std::unordered_map<FTDS::String, FTPremade*>::const_iterator premadeIter;
		premadeIter = mMapPremades.begin();
		for (; premadeIter != mMapPremades.end(); ++premadeIter)
		{
			if ((*premadeIter).second)
			{
				if (ImGui::BeginListBox((*premadeIter).second->FileName().c_str(), ImVec2(-FLT_MIN, 100)))
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

	if (ImGui::TreeNode("Materials"))
	{
		std::unordered_map<FTDS::String, FTMaterial*>::const_iterator materialIter;
		materialIter = mMapMaterials.begin();
		for (; materialIter != mMapMaterials.end(); ++materialIter)
		{
			if (ImGui::BeginListBox((*materialIter).second->FileName().c_str(), ImVec2(-FLT_MIN, 100)))
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
	}*/

	if (ImGui::TreeNode("Vertex Shaders"))
	{
		GetVertexShaders()->IterateAllValues(
			[&](FTVertexShader* res) {
				if (ImGui::BeginListBox(res->FileName().C_Str()))
				{
					res->UpdateUI();
					ImGui::EndListBox();
				}
			});
		ImGui::TreePop();
	}
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

	else if (StrContains(FileTypes::CSV, format))
		return ResType::FTCSV;

	else if (StrContains(FileTypes::JSON, format))
		return ResType::FTJSON;

	else if (StrContains(FileTypes::SHADER, format))

		if (fileName.Contains(FileTypes::VERTEX_SHADER))
			return ResType::FT_VERTEX_SHADER;
		else if (fileName.Contains(FileTypes::PIXEL_SHADER))
			return ResType::FT_PIXEL_SHADER;
		else
			return ResType::UNSUPPORTED;
}

EditorResourceManager::EditorResourceManager()
{
}

EditorResourceManager::~EditorResourceManager()
{
}