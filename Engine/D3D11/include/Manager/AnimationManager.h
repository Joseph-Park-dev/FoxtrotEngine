#pragma once
#include "Plugin/D3D11Exports.h"
#include "Entity/Entity.h"

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"
#include "Manager/FTSpineLoader.h"

namespace Core
{
	class FTCore;
	class DirectoryHelper;
} // namespace Core

namespace GenericData
{
	class FTJSON;
	class FTText;
} // namespace GenericData

#ifdef FOXTROT_EDITOR
	#include "Manager/ResourceManager.h"
using GET_JSON_FUNC		  = Common::ResourcePack<GenericData::FTJSON>* (*)();
using GET_TEXT_FUNC		  = Common::ResourcePack<GenericData::FTText>* (*)();
using GET_ASSET_PATH_FUNC = Common::FTDS::String* (*)();
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	class D3D11Renderer;
	class FTSpriteAnimation;
	class FTSpineAnimation;
	struct FTSpriteAnimationDef;

	class D3D11_API AnimationManager
	{
		SINGLETON(AnimationManager)

	public:
		/// @brief Returns the spine loader used by this d3 d11 api.
		/// @return Borrowed access to the spine loader.
		FTSpineLoader* GetSpineLoader();

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		void Initialize(D3D11::D3D11Renderer* renderer);

	private:
		D3D11::D3D11Renderer* mRenderer;
		FTSpineLoader*		  mSpineLoader;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		/// @param opened Whether the editor window remains open.
		void UpdateUI(bool* opened);
		/// @brief Displays the controls used to define and create an animation resource.
		void CreateAnimationGUI();

		/// @brief Writes animation metadata to an external resource file.
		/// @param animation Animation resource or playback state.
		template <typename ANIMATION>
		void SaveAnimationAsFile(ANIMATION* animation)
		{
			std::ofstream ofs(animation->GetRelativePath()->C_Str());
			animation->SaveProperties(ofs);
			Common::FileIOHelper::SaveBufferToFile(ofs);
		}

	public:
		/// @brief Returns the jsons func getter used by this d3 d11 api.
		/// @return Borrowed access to the jsons func getter.
		/// @note Changes through the returned reference affect this object's stored state.
		GET_JSON_FUNC& GetJSONsFuncGetter();
		/// @brief Returns the texts func getter used by this d3 d11 api.
		/// @return Borrowed access to the texts func getter.
		/// @note Changes through the returned reference affect this object's stored state.
		GET_TEXT_FUNC& GetTextsFuncGetter();

	private:
		GET_JSON_FUNC		GetJSONsFunc;
		GET_TEXT_FUNC		GetTextsFunc;
		GET_ASSET_PATH_FUNC GetAssetPathFunc;

	private:
		/// @brief Loads an animation clip from its JSON description and registers it.
		/// @param resDef Resource definition containing the filename and source path.
		/// @return Created animation from json instance or resource.
		FTSpriteAnimation* CreateAnimationFromJSON(
			FTSpriteAnimationDef& resDef);

		/// @brief Builds an animation resource from Spine skeleton and atlas data.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param jsonPath Path to the JSON resource description.
		/// @param atlasPath Path to the Spine atlas file.
		/// @return Created animation from spine instance or resource.
		FTSpineAnimation* CreateAnimationFromSpine(
			Common::FTResourceDef&		resDef,
			const Common::FTDS::String* jsonPath,
			const Common::FTDS::String* atlasPath);
#endif
	};
} // namespace D3D11
