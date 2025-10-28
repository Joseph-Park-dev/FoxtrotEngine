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
#include "ResourceSystem/FTMeshGroup.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/ModelLoader.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "ResourceSystem/FTMaterials/RimMaterial.h"
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

void ResourceManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;

	mTextures		  = DBG_NEW			FTDS::HashMap<FTTexture*>;
	mTileMaps		  = DBG_NEW			FTDS::HashMap<FTTileMap*>;
	mPremades		  = DBG_NEW			FTDS::HashMap<FTPremade*>;
	mSpriteAnimations = DBG_NEW FTDS::HashMap<FTSpriteAnimation*>;
	mSpineAnimations  = DBG_NEW	 FTDS::HashMap<FTSpineAnimation*>;
	mMeshGroups		  = DBG_NEW		  FTDS::HashMap<FTMeshGroup*>;
	mVertexShaders	  = DBG_NEW	   FTDS::HashMap<FTVertexShader*>;
	mPixelShaders	  = DBG_NEW		FTDS::HashMap<FTPixelShader*>;
	mMaterials		  = DBG_NEW		   FTDS::HashMap<FTMaterial*>;
	mSounds			  = DBG_NEW			  FTDS::HashMap<Sound*>;
	mCSVs			  = DBG_NEW				FTDS::HashMap<FTCSV*>;
	mJSONs			  = DBG_NEW			   FTDS::HashMap<FTJSON*>;
	mTexts			  = DBG_NEW			   FTDS::HashMap<FTText*>;
	mFonts			  = DBG_NEW			   FTDS::HashMap<FTFont*>;
}

void ResourceManager::DeleteAll()
{
	ClearMap(mTextures);
	ClearMap(mTileMaps);
	ClearMap(mPremades);
	ClearMap(mSpriteAnimations);
	ClearMap(mSpineAnimations);
	ClearMap(mMeshGroups);
	ClearMap(mVertexShaders);
	ClearMap(mPixelShaders);
	ClearMap(mMaterials);
	ClearMap(mSounds);
	ClearMap(mCSVs);
	ClearMap(mJSONs);
	ClearMap(mTexts);
	ClearMap(mFonts);
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

FTDS::HashMap<FTTexture*>* ResourceManager::GetTextures()
{
	return mTextures;
}

FTDS::HashMap<FTTileMap*>* ResourceManager::GetTileMaps()
{
	return mTileMaps;
}

FTDS::HashMap<FTPremade*>* ResourceManager::GetPremades()
{
	return mPremades;
}

FTDS::HashMap<FTVertexShader*>* ResourceManager::GetVertexShaders()
{
	return mVertexShaders;
}

FTDS::HashMap<FTPixelShader*>* ResourceManager::GetPixelShaders()
{
	return mPixelShaders;
}

FTDS::HashMap<FTMaterial*>* ResourceManager::GetMaterials()
{
	return mMaterials;
}

FTDS::HashMap<FTMeshGroup*>* ResourceManager::GetMeshGroups()
{
	return mMeshGroups;
}

FTDS::HashMap<FTSpriteAnimation*>* ResourceManager::GetSpriteAnimations()
{
	return mSpriteAnimations;
}

FTDS::HashMap<FTSpineAnimation*>* ResourceManager::GetSpineAnimations()
{
	return mSpineAnimations;
}

FTDS::HashMap<Sound*>* ResourceManager::GetSounds()
{
	return mSounds;
}

FTDS::HashMap<FTCSV*>* ResourceManager::GetCSVs()
{
	return mCSVs;
}

FTDS::HashMap<FTJSON*>* ResourceManager::GetJSONs()
{
	return mJSONs;
}

FTDS::HashMap<FTText*>* ResourceManager::GetTexts()
{
	return mTexts;
}

FTDS::HashMap<FTFont*>* ResourceManager::GetFonts()
{
	return mFonts;
}

void ResourceManager::LoadMaterials()
{
	const char* key	 = ChunkKey::NullVal::NULL_OBJECT;
	size_t		size = 2;

	FTDS::String fileName = ChunkKey::StandardMat::STANDARD_MAT;
	fileName.Append(+FileTypes::MATERIAL);

	FTDS::String  path = ResourceManager::GetInstance()->GetPathToAsset() + fileName;
	FTResourceDef resDef{ fileName, path };

	StandardMaterial* standard = DBG_NEW StandardMaterial(resDef, mRenderer);

	mMaterials->Reserve(1);
	mMaterials->Insert(standard->GetFileName(), standard);
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
	delete mPremades;
	delete mSpriteAnimations;
	delete mSpineAnimations;
	delete mMeshGroups;
	delete mVertexShaders;
	delete mPixelShaders;
	delete mMaterials;
	delete mSounds;
	delete mCSVs;
	delete mJSONs;
	delete mTexts;
	delete mFonts;

	mTextures		  = nullptr;
	mTileMaps		  = nullptr;
	mPremades		  = nullptr;
	mSpriteAnimations = nullptr;
	mSpineAnimations  = nullptr;
	mMeshGroups		  = nullptr;
	mVertexShaders	  = nullptr;
	mPixelShaders	  = nullptr;
	mMaterials		  = nullptr;
	mSounds			  = nullptr;
	mCSVs			  = nullptr;
	mJSONs			  = nullptr;
	mTexts			  = nullptr;
	mFonts			  = nullptr;
}

ResourceManager::ResourceManager()
	: mPathToAsset()
	, mRenderer(nullptr)
	, mTextures(nullptr)
	, mTileMaps(nullptr)
	, mPremades(nullptr)
	, mSpriteAnimations(nullptr)
	, mSpineAnimations(nullptr)
	, mMeshGroups(nullptr)
	, mVertexShaders(nullptr)
	, mPixelShaders(nullptr)
	, mMaterials(nullptr)
	, mSounds(nullptr)
	, mCSVs(nullptr)
	, mJSONs(nullptr)
	, mTexts(nullptr)
	, mFonts(nullptr)
{
}

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
	LoadGraphicsResourceFromChunk<FTTexture>(ifs, mTextures, desc.first, mRenderer);

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

void ResourceManager::LoadDefaultResources()
{
	// Defualt resources don't require file name & relative path, since they are generated from code.
	FTResourceDef resDef{
		ChunkKey::PRIMITIVE_SQUARE_SPRITE, ChunkKey::NullVal::NULL_OBJECT
	};

	FTMeshData* meshData = GeometryGenerator::MakeSquare(1.0f, FTVector3(0.f, 0.f, 1.f));

	FTMeshGroup* meshGroup = DBG_NEW FTMeshGroup(
		resDef,
		mRenderer,
		meshData);

	mMeshGroups->Reserve(1);
	mMeshGroups->Insert(ChunkKey::PRIMITIVE_SQUARE_SPRITE, meshGroup);

	delete meshData;
}

FTTexture* ResourceManager::GetLoadedTexture(FTDS::String& key)
{
	FTDS::Record<FTTexture*>* rec = mTextures->At(key);
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

FTTileMap* ResourceManager::GetLoadedTileMap(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::TILEMAP);

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

FTPremade* ResourceManager::GetLoadedPremade(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::PREMADE);

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

FTVertexShader* ResourceManager::GetLoadedVertexShader(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SHADER);

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

FTPixelShader* ResourceManager::GetLoadedPixelShader(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SHADER);

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

FTMaterial* ResourceManager::GetLoadedMaterial(FTDS::String& key)
{
	if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
		return nullptr;

	AddFileExtensionIfNone(key, FileTypes::MATERIAL);

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

FTMeshGroup* ResourceManager::GetLoadedMesh(FTDS::String& key)
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

FTSpriteAnimation* ResourceManager::GetLoadedSpriteAnim(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SPRITE_ANIMATION);

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

FTSpineAnimation* ResourceManager::GetLoadedSpineAnim(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::SPINE_ANIMATION);

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

Sound* ResourceManager::GetLoadedSound(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::Sound::WAV);

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

FTCSV* ResourceManager::GetLoadedCSV(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::CSV);

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

FTJSON* ResourceManager::GetLoadedJSON(FTDS::String& key)
{
	AddFileExtensionIfNone(key, FileTypes::JSON);
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

FTText* ResourceManager::GetLoadedText(FTDS::String& key)
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

FTFont* ResourceManager::GetLoadedFont(FTDS::String& key)
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
