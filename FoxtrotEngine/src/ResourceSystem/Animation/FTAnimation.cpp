#include "FTAnimation.h"

const char* FTAnimation::GetName() { return mName; }
bool		FTAnimation::GetIsFinished() { return mIsFinished; }

void FTAnimation::SetName(const char* name) { mName = name; }

void FTAnimation::SetFrame(int frameNumber)
{
	mIsFinished = false;
	mCurrFrame = frameNumber;
	mAccTime = 0.f;
}

void FTAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh* mesh = GetMeshes()[frameNum];
	reinterpret_cast<AnimationFrame*>(mesh)->Duration = duration;
}

void FTAnimation::SetAnimator(Animator* animator) { mAnimator = animator; }
void FTAnimation::SetIsFinished(bool val) { mIsFinished = val; }
void FTAnimation::SetIsRepeated(bool val) { mIsRepeated = val; }

void FTAnimation::IncreaseIdx() { ++mCurrFrame; }

FTAnimation::FTAnimation(FTAnimation* other)
	: FTBasicMeshGroup()
	, mName(other->mName)
	, mMaxFrameIdx(other->mMaxFrameIdx)
	, mAnimFPS(other->mAnimFPS)
	, mIsRepeated(other->mIsRepeated)
	, mAnimator(other->mAnimator)
	, mCurrFrame(0)
	, mAccTime(0.f)
	, mIsFinished(false)
{
}

bool FTAnimation::FrameIsWithinIndexRange(int currentFrame)
{
	return 0 <= currentFrame && currentFrame <= mMaxFrameIdx;
}
