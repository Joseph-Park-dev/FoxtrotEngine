// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A manager that saves/loads FTResources referred in a .Chunk file.
///
/// In the Foxtrot Editor, this loads all supported resources in the
/// "Asset" folder, and saves the FTResources that are referred in a
/// .Chunk file or used in a Scene.
/// </summary>

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

#include "Static/HashMap.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include "imgui/FileDialog/ImGuiFileDialog.h"
	#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
	#include <imgui.h>
#endif // FOXTROT_EDITOR
#include <Components/SpineAnimator.h>

class FoxtrotRenderer;
class FTTexture;
class FTSpriteAnimation;
class FTSpineAnimation;
class FTBasicMeshGroup;
struct FTMeshData;
class FTMeshDataPack;
class FTTileMap;
class FTSpriteSheet;
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
	FTSPRITESHEET,
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

class ResourceManager
{
	SINGLETON_PROTECTED(ResourceManager)

public:
	virtual void Initialize(FoxtrotRenderer* renderer);
	virtual void DeleteAll();

	void LoadResources(std::ifstream& ifs);

public:
	FTTexture*		   GetLoadedTexture(FTDS::String& key);
	FTTileMap*		   GetLoadedTileMap(FTDS::String& key);
	FTSpriteSheet*	   GetLoadedSpriteSheet(FTDS::String& key);
	FTPremade*		   GetLoadedPremade(FTDS::String& key);
	FTVertexShader*	   GetLoadedVertexShader(FTDS::String& key);
	FTPixelShader*	   GetLoadedPixelShader(FTDS::String& key);
	FTMaterial*		   GetLoadedMaterial(FTDS::String& key);
	FTBasicMeshGroup*  GetLoadedMesh(FTDS::String& key);
	FTSpriteAnimation* GetLoadedSpriteAnim(FTDS::String& key);
	FTSpineAnimation*  GetLoadedSpineAnim(FTDS::String& key);
	Sound*			   GetLoadedSound(FTDS::String& key);
	FTCSV*			   GetLoadedCSV(FTDS::String& key);
	FTJSON*			   GetLoadedJSON(FTDS::String& key);
	FTText*			   GetLoadedText(FTDS::String& key);

	FTDS::String& GetPathToAsset();
	void		  SetPathToAsset(FTDS::String&& projectPath);

	void AbsoluteToRelativePath(FTResource* res);
	void RelativeToAbsolutePath(FTResource* res);

public:
	FTDS::HashMap<FTTexture*>*			GetTextures();
	FTDS::HashMap<FTTileMap*>*			GetTileMaps();
	FTDS::HashMap<FTSpriteSheet*>*		GetSpriteSheets();
	FTDS::HashMap<FTPremade*>*			GetPremades();
	FTDS::HashMap<FTVertexShader*>*	GetVertexShaders();
	FTDS::HashMap<FTPixelShader*>*		GetPixelShaders();
	FTDS::HashMap<FTMaterial*>*		GetMaterials();
	FTDS::HashMap<FTBasicMeshGroup*>*	GetMeshGroups();
	FTDS::HashMap<FTSpriteAnimation*>* GetSpriteAnimations();
	FTDS::HashMap<FTSpineAnimation*>*	GetSpineAnimations();
	FTDS::HashMap<Sound*>*				GetSounds();
	FTDS::HashMap<FTCSV*>*				GetCSVs();
	FTDS::HashMap<FTJSON*>*			GetJSONs();
	FTDS::HashMap<FTText*>*			GetTexts();

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

protected:
	// Manually load the required Materials. If it doesn't exist in Asset/material,
	// this creates a new material.
	virtual void LoadMaterials();

	//////////////////////////
	// Processing Resources //
	//////////////////////////
	/// Member functions for processing newly loaded resources.
protected:
	FoxtrotRenderer* GetRenderer();

private:
	FTDS::String	 mPathToAsset;
	FoxtrotRenderer* mRenderer; // For Loading FTTextures

	//////////////////////
	// Foxtrot resources//
	//////////////////////
private:
	FTDS::HashMap<FTTexture*>*			mTextures;
	FTDS::HashMap<FTTileMap*>*			mTileMaps;
	FTDS::HashMap<FTSpriteSheet*>*		mSpriteSheets;
	FTDS::HashMap<FTPremade*>*			mPremades;
	FTDS::HashMap<FTSpriteAnimation*>* mSpriteAnimations;
	FTDS::HashMap<FTSpineAnimation*>*	mSpineAnimations;

	// A mesh group usually represents a 3D model.
	FTDS::HashMap<FTBasicMeshGroup*>* mMeshGroups;

	FTDS::HashMap<FTVertexShader*>* mVertexShaders;
	FTDS::HashMap<FTPixelShader*>*	 mPixelShaders;
	FTDS::HashMap<FTMaterial*>*	 mMaterials;

	FTDS::HashMap<Sound*>* mSounds;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
private:
	FTDS::HashMap<FTCSV*>*	 mCSVs;
	FTDS::HashMap<FTJSON*>* mJSONs;
	FTDS::HashMap<FTText*>* mTexts;

private:
	template <typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		FTRESOURCE* res = DBG_NEW FTRESOURCE;
		FileIOHelper::BeginDataPackLoad(ifs);
		FileIOHelper::LoadBasicString(ifs, res->RelativePath());
		FileIOHelper::LoadBasicString(ifs, res->FileName());

		assert(0 < resMap->Capacity());
		resMap->Insert(res->FileName(), res);
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

protected:
	template <typename FTRESOURCE>
	void ProcessResources(FTCore* coreInstance, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		resMap->IterateAllValues(
			[&](FTRESOURCE* res) {
				RelativeToAbsolutePath(res);
				if (res)
					res->Process(coreInstance);
			});
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
	constexpr const char* FTTEXTURE_GROUP			= "FTTexture Group";
	constexpr const char* FTMESH_GROUP				= "FTMesh Group";
	constexpr const char* FTTILEMAP_GROUP			= "FTTileMap Group";
	constexpr const char* FTSPRITESHEET_GROUP		= "FTSpriteSheet Group";
	constexpr const char* FTPREMADE_GROUP			= "FTPremade Group";
	constexpr const char* FT_SPRITE_ANIMATION_GROUP = "FTSpriteAnimation Group";
	constexpr const char* FT_SPINE_ANIMATION_GROUP	= "FTSpineAnimation Group";

	constexpr const char* PRIMITIVE_SQUARE_RED	 = "Primitive Square Red";
	constexpr const char* PRIMITIVE_SQUARE_GREEN = "Primitive Square Green";
	constexpr const char* PRIMITIVE_SQUARE_BLUE	 = "Primitive Square Blue";

	constexpr const char* PRIMITIVE_BOX			= "Primitive Square Box";
	constexpr const char* PRIMITIVE_SQUARE_GRID = "Primitive Square Grid";
	constexpr const char* PRIMITIVE_CYLINDER	= "Primitive Cylinder";
	constexpr const char* PRIMITIVE_SPHERE		= "Primitive Sphere";
} // namespace ChunkKey