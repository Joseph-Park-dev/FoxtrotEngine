#pragma once
#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "Math.h"
#include "Tile.h"

class Tile;
class AnimatorComponent;
class Texture;

struct AnimationFrame
{
	Tile tile;
	float duration;
};

class Animation
{
public:
	void CreateFrameFromTile(Tile* tile);

public:
	std::wstring& GetName() { return mName; }
	Tile* GetTile()
	{
		if (isFinished)
			SDL_Log("Reel has finished playing");
		return &mReel[mCurrFrame].tile;
	}
	std::vector<AnimationFrame>& GetReel(){ return mReel; }

	void SetName(std::wstring name) { mName = name; }
	void SetFrame(int frameNumber) 
	{
		isFinished = false;
		mCurrFrame = frameNumber;
		mAccTime = 0.f;
	}
	void SetAnimator(AnimatorComponent* animator) { mAnimator = animator; }
	bool IsFinished() { return isFinished; }

public:
	void Update(float deltaTime);

public:
	Animation();
	Animation(const Animation& other);

private:
	AnimatorComponent*			mAnimator;
	std::wstring				mName;
	std::vector<AnimationFrame> mReel;
	int							mCurrFrame;
	float						mAnimFPS;
	float						mAccTime;
	bool						isFinished;
};

