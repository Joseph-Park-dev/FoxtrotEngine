// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Entity/Entity.h"

#include <iosfwd>
#include <assert.h>

#include "Singleton.h"
#include "FTDS/Static/FTString.h"
#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/Record.h"
#include "ResourceSystem/FTResource.h"
#include "FileSystem/FileIOHelper.h"

namespace Core
{
	/// @brief Special keys used by the chunk system to reference built-in/primitive assets.
	namespace ChunkKey
	{
		constexpr const char* PRIMITIVE_SQUARE_VTX = "Primitive Square Vertex";

		constexpr const char* PRIMITIVE_SQUARE_RED	 = "Primitive Square Red";
		constexpr const char* PRIMITIVE_SQUARE_GREEN = "Primitive Square Green";
		constexpr const char* PRIMITIVE_SQUARE_BLUE	 = "Primitive Square Blue";

		constexpr const char* PRIMITIVE_BOX			= "Primitive Square Box";
		constexpr const char* PRIMITIVE_SQUARE_GRID = "Primitive Square Grid";
		constexpr const char* PRIMITIVE_CYLINDER	= "Primitive Cylinder";
		constexpr const char* PRIMITIVE_SPHERE		= "Primitive Sphere";
	} // namespace ChunkKey

	class FoxtrotRenderer;
	class ResourceManagerBase :
		public Core ::Entity
	{
	public:
		/// @brief Load all resources referenced by an open chunk stream.
		/// Stream must be positioned at the resource section.
		// void LoadResources(std::ifstream& ifs);

		/// @brief Ensure required materials are present. Creates defaults if missing.
		// virtual void LoadMaterials();

	public:
		// ---------------------------
		// Typed resource lookup APIs
		// ---------------------------
		//	/// @return Loaded Sprite by key or nullptr if not present.
		//	virtual FTSprite* GetLoadedSprite(const FTDS::String& key);
		//	/// @return Loaded Sprite by key or nullptr if not present.
		//	virtual FTTexture* GetLoadedTexture(const FTDS::String& key);
		//	/// @return Loaded tilemap by key or nullptr if not present.
		//	virtual FTTileMap* GetLoadedTileMap(const FTDS::String& key);
		//	/// @return Loaded premade by key or nullptr if not present.
		//	virtual FTPremade* GetLoadedPremade(const FTDS::String& key);
		//	/// @return Loaded vertex shader by key or nullptr if not present.
		//	virtual D3D11PSO* GetLoadedPSO(const FTDS::String& key);
		//	/// @return Loaded material by key or nullptr if not present.
		//	virtual FTMaterial* GetLoadedMaterial(const FTDS::String& key);
		//	/// @return Loaded mesh group (3D model) by key or nullptr if not present.
		//	virtual FTMeshGroup* GetLoadedMesh(const FTDS::String& key);
		//	/// @return Loaded 2D sprite animation by key or nullptr if not present.
		//	virtual FTSpriteAnimation* GetLoadedSpriteAnim(const FTDS::String& key);
		//	/// @return Loaded Spine animation by key or nullptr if not present.
		//	virtual FTSpineAnimation* GetLoadedSpineAnim(const FTDS::String& key);
		//	/// @return Loaded sound by key or nullptr if not present.
		//	virtual Sound* GetLoadedSound(const FTDS::String& key);
		//	/// @return Loaded CSV by key or nullptr if not present.
		//	virtual FTCSV* GetLoadedCSV(const FTDS::String& key);
		//	/// @return Loaded JSON by key or nullptr if not present.
		//	virtual FTJSON* GetLoadedJSON(const FTDS::String& key);
		//	/// @return Loaded raw text by key or nullptr if not present.
		//	virtual FTText* GetLoadedText(const FTDS::String& key);
		//	/// @return Loaded font by key or nullptr if not present.
		//	virtual FTFont* GetLoadedFont(const FTDS::String& key);

		///////////////////////////
		// Save | Load resources //
		///////////////////////////

	protected:
		static void AddFileExtensionIfNone(Core::FTDS::String& key, const char* fileType);

	private:
		ResArray* mResources = nullptr;

		// FTDS::HashMap<FTPremade*>* mPremades;

		//////////////////////
		// Foxtrot resources//
		//////////////////////
		// private:
		//	// Image/2D resources
		//	FTDS::HashMap<FTSprite*>*  mSprites; // The Sprites will be added when loading SpriteRenderer Component.
		//	FTDS::HashMap<FTTileMap*>* mTileMaps;
		//	FTDS::HashMap<FTPremade*>* mPremades;
		//
		//	// Animation resources
		//	FTDS::HashMap<FTSpriteAnimation*>* mSpriteAnimations;
		//	FTDS::HashMap<FTSpineAnimation*>*  mSpineAnimations;
		//
		//	// Geometry resources
		//	FTDS::HashMap<FTMeshGroup*>* mMeshGroups; // A mesh group usually represents a 3D model.
		//
		//	// Rendering
		//	FTDS::HashMap<D3D11PSO*>*					mPSOs;
		//	FTDS::HashMap<FTMaterial*>*					mMaterials;
		//	FTDS::Array<Microsoft::WRL::ComPtr<ID3D11RasterizerState>>* mRS;
		//
		//	// Audio/Fonts
		//	FTDS::HashMap<FTFont*>* mFonts;
		//	FTDS::HashMap<Sound*>*	mSounds;
		//
		//	////////////////////////////
		//	// Generic-type resources //
		//	////////////////////////////
		// private:
		//	FTDS::HashMap<FTCSV*>*	mCSVs;
		//	FTDS::HashMap<FTJSON*>* mJSONs;
		//	FTDS::HashMap<FTText*>* mTexts;

	private:
		///// @brief Core loader for graphics resources (renderer required).
		///// Skips certain built-in primitives that should not be re-instantiated from disk.
		// template <typename FTRESOURCE>
		// void LoadResource(std::ifstream& ifs, Core::FTDS::HashMap<FTRESOURCE*>& resMap, FoxtrotRenderer* renderer)
		//{
		//	assert(renderer);

		//	Core::FileIOHelper::BeginDataPackLoad(ifs);

		//	Core::FTDS::String relPath;
		//	Core::FTDS::String fileName;
		//	Core::FileIOHelper::LoadBasicString(ifs, relPath);
		//	Core::FileIOHelper::LoadBasicString(ifs, fileName);

		//	Core::FTResourceDef resDef(fileName, relPath);
		//	FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);

		//	if (fileName.Equal(ChunkKey::PRIMITIVE_SQUARE_VTX))
		//	{
		//		delete res;
		//		res = nullptr;
		//		return;
		//	}

		//	assert(0 < resMap.Capacity());
		//	resMap.Insert(res->GetFileName(), res);
		//}
	};
	constexpr const char* GET_RES_FUNC = "GetResourceManager";
} // namespace Core