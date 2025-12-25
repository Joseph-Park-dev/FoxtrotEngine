// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

/**
 * @file
 * ResourceManager.cpp
 *
 * The ResourceManager class is responsible for loading, storing,
 * and managing the lifetime of all game assets and resources,
 * such as textures, shaders, materials, sounds, and more.
 *
 * Resources are loaded from .chunk files (containing
 * serialized resources) and stored in hash maps for quick
 * lookup by other engine systems.
 *
 * Note: This file contains platform-specific code for Windows
 * (e.g., Direct3D 11) and is tightly coupled with the Foxtrot
 * engine's rendering and file I/O subsystems.
 */

#include <unordered_map>

#include "Managers/ResourceManager.h"
#include "Managers/AnimationManager.h"
#include "ResourceSystem/ResPath.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTMeshGroup.h"
#include "ResourceSystem/FTSprite/FTSprite.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/ModelLoader.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTGeometryShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "ResourceSystem/FTMaterials/SpriteAnimMat.h"
#include "ResourceSystem/Sound/Sound.h"
#include "ResourceSystem/GenericData/FTCSV.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/FTFont/FTFont.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"

#include "Static/HashMap.h"
#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

	#include "DirectoryHelper.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

/**
 * @brief Initialize the resource manager with the engine renderer.
 *
 * This function allocates and initializes internal hash maps used to store
 * loaded resources (textures, shaders, materials, sounds, etc.) and sets
 * the renderer pointer used by graphics-related resources.
 *
 * @param renderer Pointer to the FoxtrotRenderer used to create GPU resources.
 *
 * Side-effects:
 * - Allocates multiple FTDS::HashMap containers and assigns them to member pointers.
 * - Stores the renderer pointer in mRenderer.
 */
void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	mSprites		  = DBG_NEW			 FTDS::HashMap<FTSprite*>;
	mTileMaps		  = DBG_NEW			FTDS::HashMap<FTTileMap*>;
	mPremades		  = DBG_NEW			FTDS::HashMap<FTPremade*>;
	mSpriteAnimations = DBG_NEW FTDS::HashMap<FTSpriteAnimation*>;
	mSpineAnimations  = DBG_NEW	 FTDS::HashMap<FTSpineAnimation*>;
	mMeshGroups		  = DBG_NEW		  FTDS::HashMap<FTMeshGroup*>;
	mVertexShaders	  = DBG_NEW	   FTDS::HashMap<FTVertexShader*>;
	mGeometryShaders  = DBG_NEW	 FTDS::HashMap<FTGeometryShader*>;
	mPixelShaders	  = DBG_NEW		FTDS::HashMap<FTPixelShader*>;
	mMaterials		  = DBG_NEW		   FTDS::HashMap<FTMaterial*>;
	mSounds			  = DBG_NEW			  FTDS::HashMap<Sound*>;
	mCSVs			  = DBG_NEW				FTDS::HashMap<FTCSV*>;
	mJSONs			  = DBG_NEW			   FTDS::HashMap<FTJSON*>;
	mTexts			  = DBG_NEW			   FTDS::HashMap<FTText*>;
	mFonts			  = DBG_NEW			   FTDS::HashMap<FTFont*>;
}

/**
 * @brief Delete and clear all managed resources.
 *
 * Iterates through every internal container and frees contained resource
 * objects via ClearMap helper (expected to destroy objects and clear the map).
 *
 * After this call, containers remain allocated but are empty.
 *
 * Side-effects:
 * - Calls ClearMap on each managed map.
 */
void ResourceManager::DeleteAll()
{
	ClearMap(mSprites);
	ClearMap(mTileMaps);
	ClearMap(mPremades);
	ClearMap(mSpriteAnimations);
	ClearMap(mSpineAnimations);
	ClearMap(mMeshGroups);
	ClearMap(mVertexShaders);
	ClearMap(mGeometryShaders);
	ClearMap(mPixelShaders);
	ClearMap(mMaterials);
	ClearMap(mSounds);
	ClearMap(mCSVs);
	ClearMap(mJSONs);
	ClearMap(mTexts);
	ClearMap(mFonts);
}

/**
 * @brief Get the base path to the project's Assets folder.
 *
 * The returned string is the internal mPathToAsset reference.
 *
 * @return Reference to the internal FTDS::String containing the path to Assets.
 */
FTDS::String& ResourceManager::GetPathToAsset()
{
	return mPathToAsset;
}

/**
 * @brief Set the base path to the project and append the Assets folder.
 *
 * Accepts an rvalue `projectPath` and assigns it to the internal path,
 * then appends "\Assets\" so subsequent path operations assume that
 * mPathToAsset ends with the Assets folder.
 *
 * @param projectPath Project base path (moved into internal string).
 *
 * Side-effects:
 * - Changes mPathToAsset to `projectPath + "\Assets\"`.
 */
void ResourceManager::SetPathToAsset(FTDS::String&& projectPath)
{
	mPathToAsset.Assign(projectPath);
	mPathToAsset.Append("\\Assets\\");
}

/**
 * @brief Convert an absolute path to a project-relative path that begins with ".\Assets\".
 *
 * If `absPath` is already relative (starts with ".\") the function returns immediately.
 * Otherwise attempts to locate the mPathToAsset prefix inside `absPath` and reconstructs
 * a relative path starting with "." plus the subpath beginning at "\Assets\".
 *
 * @param absPath Reference to a string containing an absolute path; will be replaced
 *                with a relative path on success.
 *
 * Notes:
 * - If the mPathToAsset prefix is not found the function leaves `absPath` unchanged.
 */
void ResourceManager::AbsoluteToRelativePath(FTDS::String& absPath)
{
	FTDS::String path		= absPath;
	FTDS::String folderName = "\\Assets\\";

	// Check if the path is relative.
	if (path.LFind(".\\") == 0)
		return;

	int index = path.LFind(mPathToAsset.C_Str());
	if (index == -1)
		return;

	int cutIndex = path.RFind(folderName.C_Str());
	path.SubStr(cutIndex, path.Length());

	FTDS::String result = ".";
	result.Append(path);

	absPath = result;
}

/**
 * @brief Convert a project-relative path beginning with ".\Assets\" to an absolute path.
 *
 * If `relPath` does not start with ".\" the function returns immediately.
 * On success the leading ".\Assets\" portion is removed and the remaining path
 * is appended to mPathToAsset to produce an absolute path.
 *
 * @param relPath Reference to a string containing a relative path; will be replaced
 *                with an absolute path on success.
 */
void ResourceManager::RelativeToAbsolutePath(FTDS::String& relPath)
{
	FTDS::String path		= relPath;
	FTDS::String folderName = ".\\Assets\\";

	if (path.LFind(".\\") != 0)
		return;

	path.SubStr(folderName.Length(), path.Length());

	FTDS::String result = mPathToAsset;
	// result.Append("\\");
	result.Append(path);

	relPath = result;
}

/**
 * @brief Get pointer to the internal texture hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTTexture* records.
 */
FTDS::HashMap<FTSprite*>* ResourceManager::GetSprites()
{
	return mSprites;
}

/**
 * @brief Get pointer to the internal tilemap hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTTileMap* records.
 */
FTDS::HashMap<FTTileMap*>* ResourceManager::GetTileMaps()
{
	return mTileMaps;
}

/**
 * @brief Get pointer to the internal premade asset hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTPremade* records.
 */
FTDS::HashMap<FTPremade*>* ResourceManager::GetPremades()
{
	return mPremades;
}

/**
 * @brief Get pointer to the internal vertex shader hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTVertexShader* records.
 */
FTDS::HashMap<FTVertexShader*>* ResourceManager::GetVertexShaders()
{
	return mVertexShaders;
}

/**
 * @brief Get pointer to the internal geometry shader hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTGeometryShader* records.
 */
FTDS::HashMap<FTGeometryShader*>* ResourceManager::GetGeometryShaders()
{
	return mGeometryShaders;
}

/**
 * @brief Get pointer to the internal pixel shader hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTPixelShader* records.
 */
FTDS::HashMap<FTPixelShader*>* ResourceManager::GetPixelShaders()
{
	return mPixelShaders;
}

/**
 * @brief Get pointer to the internal material hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTMaterial* records.
 */
FTDS::HashMap<FTMaterial*>* ResourceManager::GetMaterials()
{
	return mMaterials;
}

/**
 * @brief Get pointer to the internal mesh group hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTMeshGroup* records.
 */
FTDS::HashMap<FTMeshGroup*>* ResourceManager::GetMeshGroups()
{
	return mMeshGroups;
}

/**
 * @brief Get pointer to the internal sprite animation hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTSpriteAnimation* records.
 */
FTDS::HashMap<FTSpriteAnimation*>* ResourceManager::GetSpriteAnimations()
{
	return mSpriteAnimations;
}

/**
 * @brief Get pointer to the internal spine animation hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTSpineAnimation* records.
 */
FTDS::HashMap<FTSpineAnimation*>* ResourceManager::GetSpineAnimations()
{
	return mSpineAnimations;
}

/**
 * @brief Get pointer to the internal sound hash map.
 *
 * @return Pointer to FTDS::HashMap containing Sound* records.
 */
FTDS::HashMap<Sound*>* ResourceManager::GetSounds()
{
	return mSounds;
}

/**
 * @brief Get pointer to the internal CSV data hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTCSV* records.
 */
FTDS::HashMap<FTCSV*>* ResourceManager::GetCSVs()
{
	return mCSVs;
}

/**
 * @brief Get pointer to the internal JSON data hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTJSON* records.
 */
FTDS::HashMap<FTJSON*>* ResourceManager::GetJSONs()
{
	return mJSONs;
}

/**
 * @brief Get pointer to the internal text asset hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTText* records.
 */
FTDS::HashMap<FTText*>* ResourceManager::GetTexts()
{
	return mTexts;
}

/**
 * @brief Get pointer to the internal font asset hash map.
 *
 * @return Pointer to FTDS::HashMap containing FTFont* records.
 */
FTDS::HashMap<FTFont*>* ResourceManager::GetFonts()
{
	return mFonts;
}

/**
 * @brief Load built-in material instances and insert them into the material map.
 *
 * This function creates default StandardMaterial and SpriteAnimMat instances.
 * Their resource definitions use the project's path to construct absolute paths
 * (for editor/inspection purposes) but the assets are generated from code.
 *
 * Side-effects:
 * - Allocates material objects and inserts them into mMaterials.
 * - Reserves space in mMaterials for known size (2).
 */
void ResourceManager::LoadMaterials()
{
	const char* key	 = ChunkKey::NullVal::NULL_OBJECT;
	size_t		size = 2;

	FTDS::String fileName = ChunkKey::StandardMat::STANDARD_MAT;
	fileName.Append(+FileTypes::MATERIAL);

	FTDS::String  path = ResourceManager::GetInstance()->GetPathToAsset() + fileName;
	FTResourceDef resDef{ fileName, path };

	StandardMaterial* standard = DBG_NEW StandardMaterial(resDef, mRenderer);

	fileName.Clear();
	fileName = ChunkKey::SpriteAnimMat::SPRITE_ANIM_MAT;
	fileName.Append(+FileTypes::MATERIAL);

	path.Clear();
	path   = ResourceManager::GetInstance()->GetPathToAsset() + fileName;
	resDef = FTResourceDef{ fileName, path };

	SpriteAnimMat* spriteAnim = DBG_NEW SpriteAnimMat(resDef, mRenderer);

	mMaterials->Reserve(size);
	mMaterials->Insert(standard->GetFileName(), standard);
	mMaterials->Insert(spriteAnim->GetFileName(), spriteAnim);
}

/**
 * @brief Return the renderer associated with the ResourceManager.
 *
 * @return Pointer to FoxtrotRenderer stored in the manager (may be nullptr if not initialized).
 */
FoxtrotRenderer* ResourceManager::GetRenderer()
{
	return mRenderer;
}

/**
 * @brief Destructor - release all resources and free containers.
 *
 * Calls DeleteAll to clear contained resources and then deletes all container objects.
 * After deletion, member pointers are set to nullptr.
 */
ResourceManager::~ResourceManager()
{
	DeleteAll();

	delete mSprites;
	delete mTileMaps;
	delete mPremades;
	delete mSpriteAnimations;
	delete mSpineAnimations;
	delete mMeshGroups;
	delete mVertexShaders;
	delete mGeometryShaders;
	delete mPixelShaders;
	delete mMaterials;
	delete mSounds;
	delete mCSVs;
	delete mJSONs;
	delete mTexts;
	delete mFonts;

	mSprites		  = nullptr;
	mTileMaps		  = nullptr;
	mPremades		  = nullptr;
	mSpriteAnimations = nullptr;
	mSpineAnimations  = nullptr;
	mMeshGroups		  = nullptr;
	mVertexShaders	  = nullptr;
	mGeometryShaders  = nullptr;
	mPixelShaders	  = nullptr;
	mMaterials		  = nullptr;
	mSounds			  = nullptr;
	mCSVs			  = nullptr;
	mJSONs			  = nullptr;
	mTexts			  = nullptr;
	mFonts			  = nullptr;
}

/**
 * @brief Default constructor - initialize member pointers to null and empty path.
 *
 * Members are set to null/empty so Initialize can allocate containers when called.
 */
ResourceManager::ResourceManager()
	: mPathToAsset()
	, mRenderer(nullptr)
	, mSprites(nullptr)
	, mTileMaps(nullptr)
	, mPremades(nullptr)
	, mSpriteAnimations(nullptr)
	, mSpineAnimations(nullptr)
	, mMeshGroups(nullptr)
	, mVertexShaders(nullptr)
	, mGeometryShaders(nullptr)
	, mPixelShaders(nullptr)
	, mMaterials(nullptr)
	, mSounds(nullptr)
	, mCSVs(nullptr)
	, mJSONs(nullptr)
	, mTexts(nullptr)
	, mFonts(nullptr)
{
}

/**
 * @brief Load resources from a packed data file stream.
 *
 * This routine first clears existing resources, loads default (code-generated) ones,
 * then reads multiple resource data packs from the provided input stream `ifs`.
 * Each pack is located via FileIOHelper::BeginDataPackLoad and the appropriate loader
 * template is dispatched (LoadResourceFromChunk or LoadGraphicsResourceFromChunk).
 *
 * @param ifs Input file stream positioned at the start of the resource data pack.
 *
 * Side-effects:
 * - Allocates and inserts resource objects into the manager's maps.
 * - May create GPU resources using the stored mRenderer for graphics resources.
 *
 * Notes:
 * - The function assumes `ifs` is valid and open; file error handling occurs in lower-level helpers.
 */
void ResourceManager::LoadResources(std::ifstream& ifs)
{
	DeleteAll();

	LoadDefaultResources();

	std::pair<size_t, FTDS::String> resPack	  = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RESOURCE_DATA);
	size_t							packCount = resPack.first;

	std::pair<size_t, FTDS::String> desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTText::FT_TEXT);
	LoadResourceFromChunk<FTText>(ifs, mTexts, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	LoadResourceFromChunk<FTCSV>(ifs, mCSVs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	LoadResourceFromChunk<FTJSON>(ifs, mJSONs, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Sound::SOUND);
	LoadResourceFromChunk<Sound>(ifs, mSounds, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	LoadGraphicsResourceFromChunk<FTPixelShader>(ifs, mPixelShaders, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
	LoadGraphicsResourceFromChunk<FTVertexShader>(ifs, mVertexShaders, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMeshGroup::FT_MESH_GROUP);
	LoadGraphicsResourceFromChunk<FTMeshGroup>(ifs, mMeshGroups, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTexture::FT_TEXTURE);
	LoadGraphicsResourceFromChunk<FTSprite>(ifs, mSprites, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTileMap::FT_TILEMAP);
	LoadResourceFromChunk<FTTileMap>(ifs, mTileMaps, desc.first);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
	LoadGraphicsResourceFromChunk<FTSpriteAnimation>(ifs, mSpriteAnimations, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
	LoadGraphicsResourceFromChunk<FTSpineAnimation>(ifs, mSpineAnimations, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTFont::FTFONT);
	LoadGraphicsResourceFromChunk<FTFont>(ifs, mFonts, desc.first, mRenderer);

	desc = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPremade::FT_PREMADE);
	LoadResourceFromChunk<FTPremade>(ifs, mPremades, desc.first);

	LoadMaterials();
}

/**
 * @brief Create and insert minimal default resources generated by code.
 *
 * Currently creates a single square mesh (used as a primitive sprite) and inserts
 * it into the mesh group map under ChunkKey::PRIMITIVE_SQUARE_SPRITE.
 *
 * Side-effects:
 * - Allocates FTMeshData and FTMeshGroup objects.
 * - Inserts the mesh group into mMeshGroups.
 */
void ResourceManager::LoadDefaultResources()
{
	FTResourceDef resDef{
		ChunkKey::NullVal::NULL_OBJECT, ChunkKey::NullVal::NULL_OBJECT
	};

	///////////////////////////////
	///// Vertex Shader Setup /////
	///////////////////////////////

	mVertexShaders->Reserve(5);

	// 2D sprite animation vertex shader
	FTDS::String vsPath = Path::Resource::SHADERS_2D;
	resDef.FileName		= "SpriteVS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mVertexShaders->Insert(
		resDef.FileName,
		DBG_NEW FTVertexShader(resDef, mRenderer));

	// 2D text renderer vertex shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "TextRenderer2DVS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mVertexShaders->Insert(
		resDef.FileName,
		DBG_NEW FTVertexShader(resDef, mRenderer));

	// 2D debug shape vertex shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "DebugShapeVS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mVertexShaders->Insert(
		resDef.FileName,
		DBG_NEW FTVertexShader(resDef, mRenderer));

	// 2D texture vertex shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "TextureVS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mVertexShaders->Insert(
		resDef.FileName,
		DBG_NEW FTVertexShader(resDef, mRenderer));

	// 2D spine animation vertex shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "TextureVSSpine.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mVertexShaders->Insert(
		resDef.FileName,
		DBG_NEW FTVertexShader(resDef, mRenderer));

	//////////////////////////////
	///// Pixel Shader Setup /////
	//////////////////////////////

	mPixelShaders->Reserve(6);

	// 2D sprite animation pixel shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "SpritePS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	// 2D text renderer pixel shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "TextRenderer2DPS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	// 2D texture pixel shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "Texture2DPS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	// 2D spine animation pixel shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "Texture2DSpinePS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	// 2D debug shape pixel shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "DebugShapePS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	// 2D texture pixel shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "TexturePS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mPixelShaders->Insert(
		resDef.FileName,
		DBG_NEW FTPixelShader(resDef, mRenderer));

	/////////////////////////////////
	///// Geometry Shader Setup /////
	/////////////////////////////////

	mGeometryShaders->Reserve(2);

	// 2D sprite animation geometry shader
	vsPath			= Path::Resource::SHADERS_2D;
	resDef.FileName = "SpriteGS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mGeometryShaders->Insert(
		resDef.FileName,
		DBG_NEW FTGeometryShader(resDef, mRenderer));

	// 2D debug shape geometry shader
	vsPath			= Path::Resource::SHADERS;
	resDef.FileName = "DebugShapeGS.hlsl";
	vsPath.Append(resDef.FileName);
	resDef.RelativePath = vsPath.C_Str();

	mGeometryShaders->Insert(
		resDef.FileName,
		DBG_NEW FTGeometryShader(resDef, mRenderer));
}

/**
 * @brief Retrieve a loaded texture by key.
 *
 * @param key Reference to an FTDS::String key identifying the texture in the texture map.
 * @return Pointer to FTTexture if found; otherwise nullptr.
 *
 * Notes:
 * - On failure the function logs an error via Debug::LogError.
 * - In editor builds (FOXTROT_EDITOR) the returned resource's reference count is incremented.
 */

FTSprite* ResourceManager::GetLoadedSprite(const FTDS::String& key)
{
	if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
		return nullptr;

	FTDS::Record<FTSprite*>* rec = mSprites->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

FTTexture* ResourceManager::GetLoadedTexture(const FTDS::String& key)
{
	if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
		return nullptr;

	FTDS::Record<FTSprite*>* rec = mSprites->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value()->GetTexture();
}

/**
 * @brief Retrieve a loaded tile map by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the tilemap.
 * @return Pointer to FTTileMap if found; otherwise nullptr.
 *
 * Side-effects:
 * - Potentially modifies `key` by adding default file extension.
 */
FTTileMap* ResourceManager::GetLoadedTileMap(const FTDS::String& key)
{
	FTDS::Record<FTTileMap*>* rec = mTileMaps->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded premade asset by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the premade asset.
 * @return Pointer to FTPremade if found; otherwise nullptr.
 */
FTPremade* ResourceManager::GetLoadedPremade(const FTDS::String& key)
{
	FTDS::Record<FTPremade*>* rec = mPremades->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded vertex shader by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the vertex shader.
 * @return Pointer to FTVertexShader if found; otherwise nullptr.
 */
FTVertexShader* ResourceManager::GetLoadedVertexShader(const FTDS::String& key)
{
	FTDS::Record<FTVertexShader*>* rec = mVertexShaders->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

FTGeometryShader* ResourceManager::GetLoadedGeometryShader(const FTDS::String& key)
{
	FTDS::Record<FTGeometryShader*>* rec = mGeometryShaders->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded pixel shader by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the pixel shader.
 * @return Pointer to FTPixelShader if found; otherwise nullptr.
 */
FTPixelShader* ResourceManager::GetLoadedPixelShader(const FTDS::String& key)
{
	FTDS::Record<FTPixelShader*>* rec = mPixelShaders->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded material by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the material.
 * @return Pointer to FTMaterial if found; otherwise nullptr.
 *
 * Notes:
 * - If `key` equals ChunkKey::NullVal::NULL_OBJECT, returns nullptr immediately.
 */
FTMaterial* ResourceManager::GetLoadedMaterial(const FTDS::String& key)
{
	if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
		return nullptr;

	FTDS::Record<FTMaterial*>* rec = mMaterials->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded mesh group by key.
 *
 * @param key Reference to an FTDS::String key identifying the mesh group.
 * @return Pointer to FTMeshGroup if found; otherwise nullptr.
 *
 * Notes:
 * - If `key` equals ChunkKey::NullVal::NULL_OBJECT, returns nullptr immediately.
 */
FTMeshGroup* ResourceManager::GetLoadedMesh(const FTDS::String& key)
{
	if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
		return nullptr;

	FTDS::Record<FTMeshGroup*>* rec = mMeshGroups->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded sprite animation by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the sprite animation.
 * @return Pointer to FTSpriteAnimation if found; otherwise nullptr.
 */
FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(const FTDS::String& key)
{
	FTDS::Record<FTSpriteAnimation*>* rec = mSpriteAnimations->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded spine animation by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the spine animation.
 * @return Pointer to FTSpineAnimation if found; otherwise nullptr.
 */
FTSpineAnimation* ResourceManager::GetLoadedSpineAnim(const FTDS::String& key)
{
	FTDS::Record<FTSpineAnimation*>* rec = mSpineAnimations->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded sound by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the sound.
 * @return Pointer to Sound if found; otherwise nullptr.
 */
Sound* ResourceManager::GetLoadedSound(const FTDS::String& key)
{
	FTDS::Record<Sound*>* rec = mSounds->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded CSV asset by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the CSV.
 * @return Pointer to FTCSV if found; otherwise nullptr.
 */
FTCSV* ResourceManager::GetLoadedCSV(const FTDS::String& key)
{
	FTDS::Record<FTCSV*>* rec = mCSVs->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded JSON asset by key (adds extension if missing).
 *
 * @param key Reference to an FTDS::String key identifying the JSON.
 * @return Pointer to FTJSON if found; otherwise nullptr.
 *
 * Notes:
 * - If the JSON map is empty this returns nullptr (no JSON assets loaded).
 */
FTJSON* ResourceManager::GetLoadedJSON(const FTDS::String& key)
{
	if (mJSONs->GetSize() < 1)
		return nullptr;

	FTDS::Record<FTJSON*>* rec = mJSONs->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded text asset by key.
 *
 * @param key Reference to an FTDS::String key identifying the text asset.
 * @return Pointer to FTText if found; otherwise nullptr.
 */
FTText* ResourceManager::GetLoadedText(const FTDS::String& key)
{
	FTDS::Record<FTText*>* rec = mTexts->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}

/**
 * @brief Retrieve a loaded font asset by key.
 *
 * @param key Reference to an FTDS::String key identifying the font asset.
 * @return Pointer to FTFont if found; otherwise nullptr.
 */
FTFont* ResourceManager::GetLoadedFont(const FTDS::String& key)
{
	FTDS::Record<FTFont*>* rec = mFonts->At(key);
	if (!rec)
	{
		Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
		return nullptr;
	}

#ifdef FOXTROT_EDITOR
	rec->Value()->AddRefCount();
#endif // FOXTROT_EDITOR

	return rec->Value();
}