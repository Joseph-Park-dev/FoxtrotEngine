#include "Animation.h"

void Animation::CreateFrameFromTile(Tile* tile)
{
	AnimationFrame frame = {};
	frame.tile = *tile;
	frame.duration = mAnimFPS/60.f;
	mReel.push_back(frame);
}

void Animation::Update(float deltaTime)
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

Animation::Animation()
	: mAnimator(nullptr)
	, mCurrFrame(0)
	, mAnimFPS(60.0f)
	, mAccTime(0.f)
	, isFinished(true)
{
}

Animation::Animation(const Animation& other)
{
	mAnimator = other.mAnimator;
	mName = other.mName;
}
