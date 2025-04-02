#pragma once
#include "Core/SingletonMacro.h"

class FTSpriteAnimation;

class AnimationManager
{
	SINGLETON(AnimationManager)

public:
	FTSpriteAnimation* CreateAnimationFromTile(
		const char* name,
		UINT		texKey,
		UINT		tileMapKey);
};