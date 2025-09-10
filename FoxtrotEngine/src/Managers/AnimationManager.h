#pragma once
#include "Core/SingletonMacro.h"

#include <queue>
#include "spine/spine.h"

#include "Managers/FTSpineLoader.h"
#include "Static/FTString.h"

class FTSpriteAnimation;
class FoxtrotRenderer;
class FTCore;

#ifdef FOXTROT_EDITOR
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

class AnimationManager
{
	SINGLETON(AnimationManager)

public:
	spine::FTSpineLoader* GetSpineLoader();

public:
	void Initialize(FoxtrotRenderer* renderer);

private:
	FoxtrotRenderer*	  mRenderer;
	spine::FTSpineLoader* mSpineLoader;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);
	void CreateAnimationGUI();

	template <typename ANIMATION>
	void SaveAnimationAsFile(ANIMATION* animation, FTDS::String format)
	{
		std::ofstream ofs(animation->GetRelativePath().C_Str());
		animation->SaveProperties(ofs);
		FileIOHelper::SaveBufferToFile(ofs);
	}

private:
	FTSpriteAnimation* CreateAnimationFromJSON(
		FTSpriteAnimationDef& resDef);

	FTSpineAnimation* CreateAnimationFromSpine(
		const char* name,
		FTJSON*		json,
		FTText*		atlas);
#endif
};