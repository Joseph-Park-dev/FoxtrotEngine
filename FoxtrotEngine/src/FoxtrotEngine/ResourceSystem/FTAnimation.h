#pragma once
#include <vector>
#include <string>

#include "TemplateFunctions.h"
#include "FTMath.h"
#include "Tile.h"

class Tile;
class AnimatorComponent;
class FTTexture;

struct AnimationFrame
{
	Tile tile;
	float duration;
};

class FTAnimation
{
public:
	void CreateFrameFromTile(Tile* tile);

public:
	std::wstring& GetName() { return mName; }
	Tile* GetTile()
	{
		if (isFinished)
			LogString("Reel has finished playing");
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
	FTAnimation();
	FTAnimation(const FTAnimation& other);

private:
	AnimatorComponent*			mAnimator;
	std::wstring				mName;
	std::vector<AnimationFrame> mReel;
	int							mCurrFrame;
	float						mAnimFPS;
	float						mAccTime;
	bool						isFinished;
};

