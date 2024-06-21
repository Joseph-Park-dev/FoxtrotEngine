#include "FoxtrotEngine/ResourceSystem/FTAnimation.h"

void FTAnimation::CreateFrameFromTile(Tile* tile)
{
	AnimationFrame frame = {};
	frame.tile = *tile;
	frame.duration = mAnimFPS/60.f;
	mReel.push_back(frame);
}

void FTAnimation::Update(float deltaTime)
{
	if (isFinished)
		return;
	mAccTime += deltaTime;
	if (mReel[mCurrFrame].duration < mAccTime)
	{
		++mCurrFrame;
		if (mReel.size() <= mCurrFrame)
		{
			mCurrFrame = -1;
			isFinished = true;
			mAccTime = 0.f;
			return;
		}
		mAccTime -= mReel[mCurrFrame].duration;
	}
}

FTAnimation::FTAnimation()
	: mAnimator(nullptr)
	, mCurrFrame(0)
	, mAnimFPS(60.0f)
	, mAccTime(0.f)
	, isFinished(true)
{
}

FTAnimation::FTAnimation(const FTAnimation& other)
{
	mAnimator = other.mAnimator;
	mName = other.mName;
}
