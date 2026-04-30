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
	using ResArray = FTDS::Array<FTDS::HashMap<FTResource*>>;

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

		/// @brief Load engine default/built-in resources (fonts, primitives, fallback materials, etc.).
		virtual void LoadDefaultResources() = 0;

		void DeleteAll()
		{
			for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
			{
				for (auto res = (*iter).Begin(); res != (*iter).End(); ++res)
				{
					delete (*res)->Value();
					(*res)->Value() = nullptr;
				}
			}
			mResources->Clear();
			delete mResources;
			mResources = nullptr;
		}

	public:
		void InitResMap(size_t resTypeCount)
		{
			mResources = DBG_NEW FTDS::Array<FTDS::HashMap<FTResource*>>;
			mResources->Reserve(resTypeCount);
		}

		ResArray* GetResArray();

		template <typename FTRESOURCE>
		Core::FTDS::HashMap<FTResource*>& GetResMap()
		{
			return mResources->At(FTRESOURCE::Type);
		}

		template <typename TYPE>
		TYPE* GetResource(Core::FTDS::String& key)
		{
			if (key.Equal(Core::ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			Core::FTDS::Record<Core::FTResource*>* rec = mResources->At(TYPE::Type).At(key);
			if (!rec)
			{
				Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return static_cast<TYPE*>(rec->Value());
		}

		template <typename TYPE>
		TYPE* GetResource(const char* key)
		{
			if (Core::FTDS::StringEqual(key, Core::ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			Core::FTDS::Record<Core::FTResource*>* rec = mResources->At(TYPE::Type).At(key);
			if (!rec)
			{
				Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return static_cast<TYPE*>(rec->Value());
		}

		template <typename TYPE>
		const Core::FTDS::String& GetResName(size_t typeIdx, TYPE* res)
		{
			Core::FTDS::HashMap<Core::FTResource*>& resMap = mResources->At(typeIdx);
			for (auto iter = resMap.Begin(); iter != resMap.End(); ++iter)
			{
				if (static_cast<TYPE*>((*iter)->Value()) == res)
					return (*iter)->Key();
			}
			return Core::ChunkKey::NullVal::NULL_OBJECT;
		}

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

	public:
		template <typename FTRESOURCE>
		void SaveResourcesToChunk(std::ofstream& ofs)
		{
			Core::ResArray*			   resArr = this->GetResArray();
			FTDS::HashMap<FTResource*>& map	  = GetResMap<FTRESOURCE>();
			for (auto iter = map.Begin(); iter != map.End(); ++iter)
				(*iter)->Value()->SaveProperties(ofs);
		}

		template <typename FTRESOURCE>
		void LoadResourcesFromChunk(std::ifstream& ifs, size_t& resCount)
		{
			if (resCount < 1)
				return;

			GetResMap<FTRESOURCE>().Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource<FTRESOURCE>(ifs, GetResMap<FTRESOURCE>());
				--resCount; // Key of the next resource to be imported.
			}
		}

		/// @brief Load graphics resources that require a renderer (textures, shaders, etc.).
		/// @tparam FTRESOURCE Resource concrete type with ctor(FTResourceDef, FoxtrotRenderer*).
		/// @param renderer Valid renderer used to initialize GPU-backed resources.
		template <typename FTRESOURCE>
		void LoadGraphicsResourcesFromChunk(std::ifstream& ifs, size_t& resCount, FoxtrotRenderer* renderer)
		{
			if (resCount < 1)
				return;

			GetResMap<FTRESOURCE>().Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource(ifs, GetResMap<FTRESOURCE>(), renderer);
				--resCount; // Key of the next resource to be imported.
			}
		}

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
		/// @brief Core loader for non-graphics resources (no renderer required).
		/// Expects two strings in the stream: relative path then file name.
		/// Constructs FTRESOURCE with FTResourceDef{fileName, relPath} and inserts to map keyed by file name.
		template <typename FTRESOURCE>
		void LoadResource(std::ifstream& ifs, Core::FTDS::HashMap<Core::FTResource*>& resMap)
		{
			Core::FileIOHelper::BeginDataPackLoad(ifs);

			Core::FTDS::String relPath;
			Core::FTDS::String fileName;
			Core::FileIOHelper::LoadBasicString(ifs, relPath);
			Core::FileIOHelper::LoadBasicString(ifs, fileName);

			Core::FTResourceDef resDef(fileName, relPath);
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);

			assert(0 < resMap.Capacity());
			resMap.Insert(*res->GetFileName(), res);
		}

		/// @brief Core loader for graphics resources (renderer required).
		/// Skips certain built-in primitives that should not be re-instantiated from disk.
		template <typename FTRESOURCE>
		void LoadResource(std::ifstream& ifs, Core::FTDS::HashMap<FTRESOURCE*>& resMap, FoxtrotRenderer* renderer)
		{
			assert(renderer);

			Core::FileIOHelper::BeginDataPackLoad(ifs);

			Core::FTDS::String relPath;
			Core::FTDS::String fileName;
			Core::FileIOHelper::LoadBasicString(ifs, relPath);
			Core::FileIOHelper::LoadBasicString(ifs, fileName);

			Core::FTResourceDef resDef(fileName, relPath);
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);

			if (fileName.Equal(ChunkKey::PRIMITIVE_SQUARE_VTX))
			{
				delete res;
				res = nullptr;
				return;
			}

			assert(0 < resMap.Capacity());
			resMap.Insert(res->GetFileName(), res);
		}

		/// @brief Utility to delete all values and clear a pointer map (safe if already empty).
		/// @details Template is intended to be instantiated with pointer types, e.g., ClearMap<FTTexture*>(...).
		template <typename FTRESOURCE>
		void ClearMap(Core::FTDS::HashMap<FTRESOURCE>* resMap)
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
	};
} // namespace Core