#pragma once
#include "Entity/Entity.h"

#include "SingletonMacro.h"
#include "Static/FTString.h"
#include "Manager/FTSpineLoader.h"

namespace Core
{
	class FTCore;
	class FTJSON;
	class FTText;
} // namespace Core

#ifdef FOXTROT_EDITOR
	#include "Manager/ResourceManager.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	class D3D11Renderer;
	class FTSpriteAnimation;
	class FTSpineAnimation;
	struct FTSpriteAnimationDef;

	class AnimationManager :
		public Core::Entity
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
			Core::FileIOHelper::SaveBufferToFile(ofs);
		}

	private:
		FTSpriteAnimation* CreateAnimationFromJSON(
			FTSpriteAnimationDef& resDef);

		FTSpineAnimation* CreateAnimationFromSpine(
			Core::FTResourceDef&	  resDef,
			const Core::FTDS::String* jsonPath,
			const Core::FTDS::String* atlasPath);
#endif
	};
} // namespace D3D11