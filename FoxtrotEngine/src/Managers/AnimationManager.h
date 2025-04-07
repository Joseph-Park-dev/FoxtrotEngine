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
	FTSpriteAnimation* CreateAnimationFromTile(
		const char* name,
		UINT		texKey,
		UINT		tileMapKey);

public:
	void Initialize(FoxtrotRenderer* renderer);

private:
	FoxtrotRenderer* mRenderer;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);
	void CreateAnimation();

private:
	void GetSprite(UINT& key);
	void GetTileMap(UINT& key);

	void SaveSpriteAnimAsFile(FTSpriteAnimation* animation, UINT key);
#endif
};