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
#include <string>
#include <unordered_map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Core/SingletonMacro.h"
#include "Debugging/DebugMemAlloc.h"
#include "Core/TemplateFunctions.h"
#include "ResourceSystem/FTResource.h"
#include "FileSystem/NullKeys.h"
#include "FileSystem/FileTypes.h"

#include "Static/Array.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include "imgui/FileDialog/ImGuiFileDialog.h"
	#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
	#include <imgui.h>
#endif // FOXTROT_EDITOR

class FoxtrotRenderer;
class FTTexture;
class FTSpriteAnimation;
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
class FTCSV;
class FTJSON;
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
	FTCSV,
	FTJSON
};

class ResourceManager
{
	SINGLETON_PROTECTED(ResourceManager)

public:
	virtual void Initialize(FoxtrotRenderer* renderer);
	virtual void DeleteAll();

	void SaveResources(std::ofstream& ofs);
	void LoadResources(std::ifstream& ifs, FTCore* ftCoreInst);

public:
	virtual FTTexture*		   GetLoadedTexture(const UINT key);
	virtual FTTexture*		   GetLoadedTexture(const char* fileName);
	virtual FTTileMap*		   GetLoadedTileMap(const UINT key);
	virtual FTSpriteSheet*	   GetLoadedSpriteSheet(const UINT key);
	virtual FTPremade*		   GetLoadedPremade(const UINT key);
	virtual FTPremade*		   GetLoadedPremade(const char* fileName);
	virtual FTPixelShader*	   GetLoadedPixelShader(const UINT key);
	virtual FTMaterial*		   GetLoadedMaterial(const UINT key);
	virtual FTBasicMeshGroup*  GetLoadedMesh(const UINT key);
	virtual FTSpriteAnimation* GetLoadedSpriteAnim(const UINT key);
	virtual FTCSV*			   GetLoadedCSV(const UINT key);
	virtual FTJSON*			   GetLoadedJSON(const UINT key);

	template <typename FTRESOURCE>
	UINT GetKey(FTRESOURCE* res, std::unordered_map<UINT, FTRESOURCE*> resArr)
	{
		typename std::unordered_map<UINT, FTRESOURCE*>::iterator iter = resArr.begin();
		for (; iter != resArr.end(); ++iter)
		{
			if ((*iter).second == res)
				return (*iter).first;
		}
		return ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	}

	std::string& GetPathToAsset();
	void		 SetPathToAsset(std::string&& projectPath);

	///////////////////////////
	// Save | Load resources //
	///////////////////////////
public:
	template <typename FTRESOURCE>
	void SaveResourceToChunk(std::ofstream& ofs, FTDS::Array<FTRESOURCE>& resArr)
	{
		for (FTRESOURCE* iter = resArr.Begin(); iter != resArr.End(); ++iter)
		{
			if (*iter)
			{
				if (0 < (*iter)->GetRefCount())
				{
					FileIOHelper::BeginDataPackSave(ofs, (*iter)->GetFileName());
					FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::KEY, resArr.IterPos());
					FileIOHelper::SaveString(ofs, ChunkKey::FILE_NAME, (*iter)->GetFileName());
					FileIOHelper::SaveString(ofs, ChunkKey::RELATIVE_PATH, (*iter)->GetRelativePath());
					FileIOHelper::EndDataPackSave(ofs, (*iter)->GetFileName());
				}
			}
		}
	}

	void SaveMaterialsToChunk(std::ofstream& ofs);

	template <typename FTRESOURCE>
	void LoadResourceFromChunk(std::ifstream& ifs, FTDS::Array<FTRESOURCE*>& resArr, size_t& resCount)
	{
		resArr.Reserve(resCount);
		while (0 < resCount)
		{
			LoadResource(ifs, resArr);
			--resCount; // Key of the next resource to be imported.
		}
		// Subtract the number of resources loaded.
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

	void ProcessTexture(FTTexture* texture);
	void ProcessSingleMeshGrp(FTBasicMeshGroup* meshGrp);
	void ProcessTileMap(FTTileMap* tileMap);
	void ProcessSpriteSheet(FTSpriteSheet* spriteSheet);
	void ProcessSpriteAnim(FTSpriteAnimation* spriteAnim);
	void ProcessCSV(FTCSV* csv);
	void ProcessJSON(FTJSON* json);

	virtual void ProcessTextures();
	virtual void ProcessMeshGroups();
	virtual void ProcessPremades();
	virtual void ProcessTileMaps();
	virtual void ProcessSpriteSheets();
	virtual void ProcessSpriteAnims();
	virtual void ProcessCSVs();
	virtual void ProcessJSONs();

	virtual void ProcessMaterials();
	virtual void ProcessVertexShaders();
	virtual void ProcessPixelShaders();

	/// Due to the abstract base type, Material loading requires dedicated functions
	void ProcessMaterial(FTMaterial* material);

private:
	UINT			 mItemKey;
	std::string		 mPathToAsset;
	FoxtrotRenderer* mRenderer; // For Loading FTTextures

	//////////////////////
	// Foxtrot resources//
	//////////////////////
private:
	FTDS::Array<FTTexture*>			mTextures;
	FTDS::Array<FTTileMap*>			mTileMaps;
	FTDS::Array<FTSpriteSheet*>		mSpriteSheets;
	FTDS::Array<FTPremade*>			mPremades;
	FTDS::Array<FTSpriteAnimation*> mSpriteAnimations;

	// A mesh group usually represents a 3D model.
	FTDS::Array<FTBasicMeshGroup*> mMeshGroups;

	FTDS::Array<FTVertexShader*> mVertexShaders;
	FTDS::Array<FTPixelShader*>	 mPixelShaders;

	FTDS::Array<FTMaterial*> mMaterials;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
private:
	FTDS::Array<FTCSV*>	 mCSVs;
	FTDS::Array<FTJSON*> mJSONs;

private:
	template <typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, FTDS::Array<FTRESOURCE*>& resArr)
	{
		FTRESOURCE* res	= DBG_NEW FTRESOURCE;
		UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;

		FileIOHelper::BeginDataPackLoad(ifs);
		FileIOHelper::LoadBasicString(ifs, res->RelativePath());
		FileIOHelper::LoadBasicString(ifs, res->FileName());
		FileIOHelper::LoadUnsignedInt(ifs, key);

		// assertion when key collision is detected.
		assert(!resArr.At(key));
		resArr[key] = res;
	}

	//////////////////////////
	// Validating Resources //
	//////////////////////////
	template <typename FTRESOURCE>
	void ClearResArray(FTDS::Array<FTRESOURCE>& resArr)
	{
		for (auto iter = resArr.Begin(); iter != resArr.End(); ++iter)
			if (iter)
				delete (*iter);
		resArr.Clear();
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

	constexpr const unsigned int PRIMITIVE_SQUARE_RED	= 1;
	constexpr const unsigned int PRIMITIVE_SQUARE_GREEN = 2;
	constexpr const unsigned int PRIMITIVE_SQUARE_BLUE	= 3;

	constexpr const unsigned int PRIMITIVE_BOX		   = 4;
	constexpr const unsigned int PRIMITIVE_SQUARE_GRID = 5;
	constexpr const unsigned int PRIMITIVE_CYLINDER	   = 6;
	constexpr const unsigned int PRIMITIVE_SPHERE	   = 7;
} // namespace ChunkKey