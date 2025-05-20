#pragma once
#include "Core/SingletonMacro.h"

#include <queue>
#include "spine/spine.h"

#include "Static/FTString.h"

class FTSpriteAnimation;
class FoxtrotRenderer;
class FTCore;

class AnimationManager
{
	SINGLETON(AnimationManager)

public:
	// FTSpriteAnimation* CreateAnimationFromTileMap(
	//	const char* name,
	//	UINT		texKey,
	//	UINT		tileMapKey);

	FTSpriteAnimation* CreateAnimationFromSpriteSheet(
		const char*	  name,
		FTDS::String& texKey,
		FTDS::String& spriteSheetKey,
		size_t		  startIndex,
		size_t		  endIndex);

public:
	spine::SkeletonRenderer* GetSkeletonRenderer();

public:
	void Initialize(FoxtrotRenderer* renderer);

private:
	FoxtrotRenderer*		 mRenderer;
	spine::SkeletonRenderer* mSkeletonRenderer;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);
	void CreateAnimation();
	void SaveSpriteAnimAsFile(FTSpriteAnimation* animation);

private:
	void GetSprite(FTDS::String& key);
	void GetTileMap(FTDS::String& key);
	void GetSpriteSheet(FTDS::String& key);
#endif
};