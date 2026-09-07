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
		FTSpineLoader* GetSpineLoader();

	public:
		void Initialize(D3D11::D3D11Renderer* renderer);

	private:
		D3D11::D3D11Renderer* mRenderer;
		FTSpineLoader*		  mSpineLoader;

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI(bool* opened);
		void CreateAnimationGUI();

		template <typename ANIMATION>
		void SaveAnimationAsFile(ANIMATION* animation)
		{
			std::ofstream ofs(animation->GetRelativePath()->C_Str());
			animation->SaveProperties(ofs);
			Common::FileIOHelper::SaveBufferToFile(ofs);
		}

	public:
		GET_JSON_FUNC& GetJSONsFuncGetter();
		GET_TEXT_FUNC& GetTextsFuncGetter();

	private:
		GET_JSON_FUNC		GetJSONsFunc;
		GET_TEXT_FUNC		GetTextsFunc;
		GET_ASSET_PATH_FUNC GetAssetPathFunc;

	private:
		FTSpriteAnimation* CreateAnimationFromJSON(
			FTSpriteAnimationDef& resDef);

		FTSpineAnimation* CreateAnimationFromSpine(
			Common::FTResourceDef&		resDef,
			const Common::FTDS::String* jsonPath,
			const Common::FTDS::String* atlasPath);
#endif
	};
} // namespace D3D11