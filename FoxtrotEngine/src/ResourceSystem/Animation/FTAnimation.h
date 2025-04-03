#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

class Animator;

struct AnimationFrame
{
	float Duration;
};

class FTAnimation : public FTBasicMeshGroup
{
public:
	const char* GetName();
	bool		GetIsFinished();

	void SetName(const char* name);
	void SetFrame(int frameNumber);
	void SetFrameDuration(int frameNum, float duration);
	void SetAnimator(Animator* animator);
	void SetIsFinished(bool val);
	void SetIsRepeated(bool val);
	void IncreaseIdx();

public:
	FTAnimation();
	FTAnimation(FTAnimation* other);
	~FTAnimation();

private:
	// These fields need to be loaded from .chunk file
	const char* mName;
	float		mAnimFPS;
	bool		mIsRepeated;
	int			mMaxFrameIdx;

	// These fields need to be initialized when the component is added.
	Animator* mAnimator;
	int		  mCurrFrame;
	float	  mAccTime;
	bool	  mIsFinished;

private:
	bool FrameIsWithinIndexRange(int currentFrame);
};
