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
	void CreateAnimation();

	template <typename ANIMATION>
	void SaveAnimationAsFile(ANIMATION* animation, FTDS::String format)
	{
		FTDS::String  path = EditorResourceManager::GetInstance()->GetPathToAsset() + animation->FileName() + format;
		std::ofstream ofs(path.C_Str());
		animation->SetRelativePath(path);
		animation->SaveProperties(ofs);
		FileIOHelper::SaveBufferToFile(ofs);
	}

private:
	void GetSprite(FTDS::String& key);
	void GetTileMap(FTDS::String& key);
	void GetSpriteSheet(FTDS::String& key);

private:
	FTSpriteAnimation* CreateAnimationFromSpriteSheet(
		const char*	  name,
		FTDS::String& texKey,
		FTDS::String& spriteSheetKey,
		size_t		  startIndex,
		size_t		  endIndex);

	FTSpineAnimation* CreateAnimationFromSpine(
		const char*	  name,
		FTDS::String& jsonKey,
		FTDS::String& atlasKey);
#endif
};