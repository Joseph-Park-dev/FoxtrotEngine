// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

// Core/engine essentials required by this header.
#include "Core/SingletonMacro.h"	   // Singleton macro for manager lifetime/creation.
#include "Debugging/DebugMemAlloc.h"   // DBG_NEW tracking.
#include "ResourceSystem/FTResource.h" // FTResourceDef and base concepts used by templates.
#include "FileSystem/FileIOHelper.h"   // Static helpers used in header-defined templates.
#include "Static/HashMap.h"			   // FTDS::HashMap used for all resource tables.
#include "Static/FTString.h"		   // FTDS::String used as keys and path buffers.

// Forward declarations to minimize compile-time coupling.
class FoxtrotRenderer;

class FTTexture;
class FTTileMap;
class FTPremade;
class FTMeshGroup;
class FTVertexShader;
class FTPixelShader;
class FTGeometryShader;
class FTMaterial;

class FTSpriteAnimation;
class FTSpineAnimation;

class Sound;
class FTCSV;
class FTJSON;
class FTText;
class FTFont;

class FileIOHelper;

/// @brief Type discriminator for resources serialized/deserialized from chunk files.
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
	FTSOUND,
	FTFONT
};

/// @brief Special keys used by the chunk system to reference built-in/primitive assets.
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

/// @brief Manages discovery, lifetime, serialization, and lookup of engine resources.
///
/// Responsibilities:
/// - On editor: scans the Asset directory, loads supported resources, and tracks usage by Scene/Chunk.
/// - On runtime: loads/saves only resources referenced by chunk/scene data.
/// - Provides typed maps and typed getter utilities.
/// - Owns resource lifetimes (DeleteAll).
class ResourceManager
{
	SINGLETON_PROTECTED(ResourceManager)

public:
	/// @brief Initialize the manager and bind the renderer for GPU resource creation.
	/// @param renderer Non-null renderer used for graphics resources.
	virtual void Initialize(FoxtrotRenderer* renderer);

	/// @brief Frees all loaded resources and clears internal maps.
	virtual void DeleteAll();

	/// @brief Load all resources referenced by an open chunk stream.
	/// Stream must be positioned at the resource section.
	void LoadResources(std::ifstream& ifs);

	/// @brief Ensure required materials are present. Creates defaults if missing.
	virtual void LoadMaterials();

	/// @brief Load engine default/built-in resources (fonts, primitives, fallback materials, etc.).
	void LoadDefaultResources();

	// ---------------------------
	// Typed resource lookup APIs
	// ---------------------------

public:
	/// @return Loaded texture by key or nullptr if not present.
	virtual FTTexture* GetLoadedTexture(FTDS::String& key);
	/// @return Loaded tilemap by key or nullptr if not present.
	virtual FTTileMap* GetLoadedTileMap(FTDS::String& key);
	/// @return Loaded premade by key or nullptr if not present.
	virtual FTPremade* GetLoadedPremade(FTDS::String& key);
	/// @return Loaded vertex shader by key or nullptr if not present.
	virtual FTVertexShader* GetLoadedVertexShader(FTDS::String& key);
	/// @return Loaded geometry shader by key or nullptr if not present.
	virtual FTGeometryShader* GetLoadedGeometryShader(FTDS::String& key);
	/// @return Loaded pixel shader by key or nullptr if not present.
	virtual FTPixelShader* GetLoadedPixelShader(FTDS::String& key);
	/// @return Loaded material by key or nullptr if not present.
	virtual FTMaterial* GetLoadedMaterial(FTDS::String& key);
	/// @return Loaded mesh group (3D model) by key or nullptr if not present.
	virtual FTMeshGroup* GetLoadedMesh(FTDS::String& key);
	/// @return Loaded 2D sprite animation by key or nullptr if not present.
	virtual FTSpriteAnimation* GetLoadedSpriteAnim(FTDS::String& key);
	/// @return Loaded Spine animation by key or nullptr if not present.
	virtual FTSpineAnimation* GetLoadedSpineAnim(FTDS::String& key);
	/// @return Loaded sound by key or nullptr if not present.
	virtual Sound* GetLoadedSound(FTDS::String& key);
	/// @return Loaded CSV by key or nullptr if not present.
	virtual FTCSV* GetLoadedCSV(FTDS::String& key);
	/// @return Loaded JSON by key or nullptr if not present.
	virtual FTJSON* GetLoadedJSON(FTDS::String& key);
	/// @return Loaded raw text by key or nullptr if not present.
	virtual FTText* GetLoadedText(FTDS::String& key);
	/// @return Loaded font by key or nullptr if not present.
	virtual FTFont* GetLoadedFont(FTDS::String& key);

	/// @brief Root absolute path to the project's Asset directory.
	/// Example: D:/Project/Assets
	FTDS::String& GetPathToAsset();

	/// @brief Set the absolute Asset directory path.
	/// @note May normalize/trailing-slash internally as needed.
	virtual void SetPathToAsset(FTDS::String&& projectPath);

	/// @brief Convert an absolute path under Assets to a relative asset path in-place.
	void AbsoluteToRelativePath(FTDS::String& absPath);
	/// @brief Convert a relative asset path to absolute path in-place.
	void RelativeToAbsolutePath(FTDS::String& relPath);

	// ---------------------------
	// Direct access to resource maps
	// ---------------------------

public:
	virtual FTDS::HashMap<FTTexture*>*		   GetTextures();
	virtual FTDS::HashMap<FTTileMap*>*		   GetTileMaps();
	virtual FTDS::HashMap<FTPremade*>*		   GetPremades();
	virtual FTDS::HashMap<FTVertexShader*>*	   GetVertexShaders();
	virtual FTDS::HashMap<FTGeometryShader*>*  GetGeometryShaders();
	virtual FTDS::HashMap<FTPixelShader*>*	   GetPixelShaders();
	virtual FTDS::HashMap<FTMaterial*>*		   GetMaterials();
	virtual FTDS::HashMap<FTMeshGroup*>*	   GetMeshGroups();
	virtual FTDS::HashMap<FTSpriteAnimation*>* GetSpriteAnimations();
	virtual FTDS::HashMap<FTSpineAnimation*>*  GetSpineAnimations();
	virtual FTDS::HashMap<Sound*>*			   GetSounds();
	virtual FTDS::HashMap<FTCSV*>*			   GetCSVs();
	virtual FTDS::HashMap<FTJSON*>*			   GetJSONs();
	virtual FTDS::HashMap<FTText*>*			   GetTexts();
	virtual FTDS::HashMap<FTFont*>*			   GetFonts();

	///////////////////////////
	// Save | Load resources //
	///////////////////////////

public:
	/// @brief Load non-graphics resource entries from a chunk stream into a typed map.
	/// @tparam FTRESOURCE Resource concrete type (e.g., FTCSV, FTJSON, FTText, etc.).
	/// @param ifs Stream positioned at the next resource entry.
	/// @param resArr Target map. Will be reserved to 'resCount'.
	/// @param resCount Number of entries to read. Decrements to 0 during import.
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

	/// @brief Load graphics resources that require a renderer (textures, shaders, etc.).
	/// @tparam FTRESOURCE Resource concrete type with ctor(FTResourceDef, FoxtrotRenderer*).
	/// @param renderer Valid renderer used to initialize GPU-backed resources.
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
	/// @brief Currently bound renderer. Not owned.
	FoxtrotRenderer* GetRenderer();

private:
	// ---------------------------
	// Paths and renderer handle
	// ---------------------------

	FTDS::String	 mPathToAsset; // Absolute path to the Assets root folder.
	FoxtrotRenderer* mRenderer;	   // Used for loading GPU-backed resources (textures, shaders, etc.).

	//////////////////////
	// Foxtrot resources//
	//////////////////////
private:
	// Image/2D resources
	FTDS::HashMap<FTTexture*>* mTextures;
	FTDS::HashMap<FTTileMap*>* mTileMaps;
	FTDS::HashMap<FTPremade*>* mPremades;

	// Animation resources
	FTDS::HashMap<FTSpriteAnimation*>* mSpriteAnimations;
	FTDS::HashMap<FTSpineAnimation*>*  mSpineAnimations;

	// Geometry resources
	FTDS::HashMap<FTMeshGroup*>* mMeshGroups; // A mesh group usually represents a 3D model.

	// Shaders/materials
	FTDS::HashMap<FTVertexShader*>*	  mVertexShaders;
	FTDS::HashMap<FTGeometryShader*>* mGeometryShaders;
	FTDS::HashMap<FTPixelShader*>*	  mPixelShaders;
	FTDS::HashMap<FTMaterial*>*		  mMaterials;

	// Audio/Fonts
	FTDS::HashMap<FTFont*>* mFonts;
	FTDS::HashMap<Sound*>*	mSounds;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
private:
	FTDS::HashMap<FTCSV*>*	mCSVs;
	FTDS::HashMap<FTJSON*>* mJSONs;
	FTDS::HashMap<FTText*>* mTexts;

private:
	/// @brief Core loader for non-graphics resources (no renderer required).
	/// Expects two strings in the stream: relative path then file name.
	/// Constructs FTRESOURCE with FTResourceDef{fileName, relPath} and inserts to map keyed by file name.
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

	/// @brief Core loader for graphics resources (renderer required).
	/// Skips certain built-in primitives that should not be re-instantiated from disk.
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

		if (fileName.Equal(ChunkKey::PRIMITIVE_SQUARE_SPRITE))
		{
			delete res;
			res = nullptr;
			return;
		}

		assert(0 < resMap->Capacity());
		resMap->Insert(res->GetFileName(), res);
	}

	/// @brief Utility to delete all values and clear a pointer map (safe if already empty).
	/// @details Template is intended to be instantiated with pointer types, e.g., ClearMap<FTTexture*>(...).
	template <typename FTRESOURCE>
	void ClearMap(FTDS::HashMap<FTRESOURCE>* resMap)
	{
		if (resMap)
		{
			if (resMap->GetSize() < 1)
				return;

			resMap->IterateAllValues(
				[&](FTRESOURCE& res) {
					if (res)
					{
						delete res;
						res = nullptr;
					}
				});
			resMap->Clear();
		}
	}

private:
	/// @brief Appends the given file extension if the key has none.
	/// @note Mutates the provided string when the suffix is missing.
	void AddFileExtensionIfNone(FTDS::String& key, const char* fileType)
	{
		if (key.RFind(fileType) < 0)
			key.Append(fileType);
	}
};