#pragma once
#include "Core/SingletonMacro.h"

#include <queue>

class FTSpriteAnimation;
class FoxtrotRenderer;
class FTCore;

class AnimationManager
{
	SINGLETON(AnimationManager)

public:
	//FTSpriteAnimation* CreateAnimationFromTileMap(
	//	const char* name,
	//	UINT		texKey,
	//	UINT		tileMapKey);

	FTSpriteAnimation* CreateAnimationFromSpriteSheet(
		const char* name, 
		UINT texKey, 
		UINT spriteSheetKey, 
		size_t startIndex, 
		size_t endIndex);

public:
	void Initialize(FoxtrotRenderer* renderer);

private:
	FoxtrotRenderer* mRenderer;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);
	void CreateAnimation();
	void SaveSpriteAnimAsFile(FTSpriteAnimation* animation, UINT key);

private:
	void GetSprite(UINT& key);
	void GetTileMap(UINT& key);
	void GetSpriteSheet(UINT& key);
#endif
};