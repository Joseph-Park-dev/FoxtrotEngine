#pragma once
#include "Entity/Entity.h"

#include "SingletonMacro.h"
#include "Static/FTString.h"
#include "Manager/FTSpineLoader.h"

namespace Core
{
	class FoxtrotRenderer;
	class FTCore;
} // namespace Core

#ifdef FOXTROT_EDITOR
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	class FTSpriteAnimation;

	class AnimationManager :
		public Core::Entity
	{
		SINGLETON(AnimationManager)

	public:
		FTSpineLoader* GetSpineLoader();

	public:
		void Initialize(Core::FoxtrotRenderer* renderer);

	protected:
		void RegisterMemberFuncs() override;

	private:
		Core::FoxtrotRenderer* mRenderer;
		FTSpineLoader*		   mSpineLoader;

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI(bool* opened);
		void CreateAnimationGUI();

		template <typename ANIMATION>
		void SaveAnimationAsFile(ANIMATION* animation)
		{
			std::ofstream ofs(animation->GetRelativePath().C_Str());
			animation->SaveProperties(ofs);
			FileIOHelper::SaveBufferToFile(ofs);
		}

	private:
		FTSpriteAnimation* CreateAnimationFromJSON(
			FTSpriteAnimationDef& resDef);

		FTSpineAnimation* CreateAnimationFromSpine(
			FTResourceDef& resDef,
			FTJSON*		   json,
			FTText*		   atlas);
#endif
	};
} // namespace D3D11