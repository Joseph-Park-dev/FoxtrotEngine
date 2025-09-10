// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <unordered_map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Core/SingletonMacro.h"
#include "Debugging/DebugMemAlloc.h"
#include "Core/TemplateFunctions.h"
#include "ResourceSystem/FTResource.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "FileSystem/NullKeys.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"

#include "Static/HashMap.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>
	#include <imgui.h>
#endif // FOXTROT_EDITOR
#include <Components/SpineAnimator.h>

class FoxtrotRenderer;
class FTTexture;
class FTSpriteAnimation;
class FTSpineAnimation;
class FTMeshGroup;
struct FTMeshData;
class FTMeshDataPack;
class FTTileMap;
class FTPremade;
class FTCore;
class FTMaterial;
class FTVertexShader;
class FTPixelShader;
class Sound;
class FTCSV;
class FTJSON;
class FTText;
class FileIOHelper;

enum class ResType
{
	UNSUPPORTED,
	FTTEXTURE,
	FT_CUBEMAP_TEXTURE,
	FTTILEMAP,
	FTPREMADE,
	FTMESH,
	FTMATERIAL,
	FT_VERTEX_SHADER,
	FT_PIXEL_SHADER,
	FT_SPRITE_ANIMATION,
	FT_SPINE_ANIMATION,
	FTCSV,
	FTJSON,
	FTTEXT,
	FT_SHADER_META,
	FTSOUND
};

/// @brief A manager that saves/loads FTResources referred in a .Chunk file.
/// On Foxtrot Editor, this loads all supported resources in the
/// "Asset" folder, and saves the FTResources that are referred in a .Chunk file or used in a Scene.
class ResourceManager
{
	SINGLETON_PROTECTED(ResourceManager)

public:
	virtual void Initialize(FoxtrotRenderer* renderer);
	virtual void DeleteAll();

	void LoadResources(std::ifstream& ifs);

	// Manually load the required Materials. If it doesn't exist in Asset/material,
	// this creates a new material.
	virtual void LoadMaterials();
	void		 LoadDefaultResources();

public:
	virtual FTTexture*		   GetLoadedTexture(FTDS::String& key);
	virtual FTTileMap*		   GetLoadedTileMap(FTDS::String& key);
	virtual FTPremade*		   GetLoadedPremade(FTDS::String& key);
	virtual FTVertexShader*	   GetLoadedVertexShader(FTDS::String& key);
	virtual FTPixelShader*	   GetLoadedPixelShader(FTDS::String& key);
	virtual FTMaterial*		   GetLoadedMaterial(FTDS::String& key);
	virtual FTMeshGroup*	   GetLoadedMesh(FTDS::String& key);
	virtual FTSpriteAnimation* GetLoadedSpriteAnim(FTDS::String& key);
	virtual FTSpineAnimation*  GetLoadedSpineAnim(FTDS::String& key);
	virtual Sound*			   GetLoadedSound(FTDS::String& key);
	virtual FTCSV*			   GetLoadedCSV(FTDS::String& key);
	virtual FTJSON*			   GetLoadedJSON(FTDS::String& key);
	virtual FTText*			   GetLoadedText(FTDS::String& key);

	FTDS::String& GetPathToAsset();
	virtual void  SetPathToAsset(FTDS::String&& projectPath);

	void AbsoluteToRelativePath(FTDS::String& absPath);
	void RelativeToAbsolutePath(FTDS::String& relPath);

public:
	virtual FTDS::HashMap<FTTexture*>*		   GetTextures();
	virtual FTDS::HashMap<FTTileMap*>*		   GetTileMaps();
	virtual FTDS::HashMap<FTPremade*>*		   GetPremades();
	virtual FTDS::HashMap<FTVertexShader*>*	   GetVertexShaders();
	virtual FTDS::HashMap<FTPixelShader*>*	   GetPixelShaders();
	virtual FTDS::HashMap<FTMaterial*>*		   GetMaterials();
	virtual FTDS::HashMap<FTMeshGroup*>*	   GetMeshGroups();
	virtual FTDS::HashMap<FTSpriteAnimation*>* GetSpriteAnimations();
	virtual FTDS::HashMap<FTSpineAnimation*>*  GetSpineAnimations();
	virtual FTDS::HashMap<Sound*>*			   GetSounds();
	virtual FTDS::HashMap<FTCSV*>*			   GetCSVs();
	virtual FTDS::HashMap<FTJSON*>*			   GetJSONs();
	virtual FTDS::HashMap<FTText*>*			   GetTexts();

	///////////////////////////
	// Save | Load resources //
	///////////////////////////
public:
	void SaveMaterialsToChunk(std::ofstream& ofs);

	template <typename FTRESOURCE>
	void LoadResourceFromChunk(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resArr, size_t& resCount)
	{
		if (resCount < 1)
			return;

		resArr->Reserve(resCount);
		while (0 < resCount)
		{
			LoadResource(ifs, resArr);
			--resCount; // Key of the next resource to be imported.
		}
	}

	template <typename FTRESOURCE>
	void LoadGraphicsResourceFromChunk(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resArr, size_t& resCount, FoxtrotRenderer* renderer)
	{
		if (resCount < 1)
			return;

		resArr->Reserve(resCount);
		while (0 < resCount)
		{
			LoadResource(ifs, resArr, renderer);
			--resCount; // Key of the next resource to be imported.
		}
	}

public:
	FoxtrotRenderer* GetRenderer();

private:
	FTDS::String	 mPathToAsset;
	FoxtrotRenderer* mRenderer; // For Loading FTTextures

	//////////////////////
	// Foxtrot resources//
	//////////////////////
private:
	FTDS::HashMap<FTTexture*>*		   mTextures;
	FTDS::HashMap<FTTileMap*>*		   mTileMaps;
	FTDS::HashMap<FTPremade*>*		   mPremades;
	FTDS::HashMap<FTSpriteAnimation*>* mSpriteAnimations;
	FTDS::HashMap<FTSpineAnimation*>*  mSpineAnimations;

	// A mesh group usually represents a 3D model.
	FTDS::HashMap<FTMeshGroup*>* mMeshGroups;

	FTDS::HashMap<FTVertexShader*>* mVertexShaders;
	FTDS::HashMap<FTPixelShader*>*	mPixelShaders;
	FTDS::HashMap<FTMaterial*>*		mMaterials;

	FTDS::HashMap<Sound*>* mSounds;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
private:
	FTDS::HashMap<FTCSV*>*	mCSVs;
	FTDS::HashMap<FTJSON*>* mJSONs;
	FTDS::HashMap<FTText*>* mTexts;

private:
	template <typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		FileIOHelper::BeginDataPackLoad(ifs);

		FTDS::String relPath;
		FTDS::String fileName;
		FileIOHelper::LoadBasicString(ifs, relPath);
		FileIOHelper::LoadBasicString(ifs, fileName);

		FTResourceDef resDef{ fileName, relPath };
		FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);

		assert(0 < resMap->Capacity());
		resMap->Insert(res->GetFileName(), res);
	}

	template <typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resMap, FoxtrotRenderer* renderer)
	{
		assert(renderer);

		FileIOHelper::BeginDataPackLoad(ifs);

		FTDS::String relPath;
		FTDS::String fileName;
		FileIOHelper::LoadBasicString(ifs, relPath);
		FileIOHelper::LoadBasicString(ifs, fileName);

		FTResourceDef resDef{ fileName, relPath };
		FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);

		assert(0 < resMap->Capacity());
		resMap->Insert(res->GetFileName(), res);
	}

	template <typename FTRESOURCE>
	void ClearMap(FTDS::HashMap<FTRESOURCE>* resMap)
	{
		if (resMap)
		{
			if (resMap->GetSize() < 1)
				return;

			resMap->IterateAllValues(
				[&](FTRESOURCE res) {
					if (res)
					{
						delete res;
						res = nullptr;
					}
				});
		}
	}

private:
	void AddFileExtensionIfNone(FTDS::String& key, const char* fileType)
	{
		if (key.RFind(fileType) < 0)
			key.Append(fileType);
	}
};

namespace ChunkKey
{
	constexpr const char* PRIMITIVE_SQUARE_SPRITE = "Primitive Square Sprite";

	constexpr const char* PRIMITIVE_SQUARE_RED	 = "Primitive Square Red";
	constexpr const char* PRIMITIVE_SQUARE_GREEN = "Primitive Square Green";
	constexpr const char* PRIMITIVE_SQUARE_BLUE	 = "Primitive Square Blue";

	constexpr const char* PRIMITIVE_BOX			= "Primitive Square Box";
	constexpr const char* PRIMITIVE_SQUARE_GRID = "Primitive Square Grid";
	constexpr const char* PRIMITIVE_CYLINDER	= "Primitive Cylinder";
	constexpr const char* PRIMITIVE_SPHERE		= "Primitive Sphere";
} // namespace ChunkKey