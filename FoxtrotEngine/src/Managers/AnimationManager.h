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
		FTDS::String path = ResourceManager::GetInstance()->GetPathToAsset() + animation->GetFileName();
		if (animation->GetFileName().RFind(format.C_Str()) == -1)
			path += format;
		std::ofstream ofs(path.C_Str());
		animation->SaveProperties(ofs);
		FileIOHelper::SaveBufferToFile(ofs);
	}

private:
	void GetSprite(FTDS::String& key);
	void GetTileMap(FTDS::String& key);
	void GetSpriteSheet(FTDS::String& key);

private:
	FTSpriteAnimation* CreateAnimationFromSpineAtlas(
		const char* name,
		FTText*		sheet,
		int			minIdx,
		int			maxIdx);

	FTSpineAnimation* CreateAnimationFromSpine(
		const char* name,
		FTJSON*		json,
		FTText*		atlas);
#endif
};