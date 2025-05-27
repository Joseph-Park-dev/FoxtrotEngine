#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include "spine/spine.h"

class FTSpineAnimation :
	public FTAnimation
{
public:
	void		 Initialize(spine::SkeletonData* skel, spine::AnimationStateData* stateData = 0);
	virtual void Update(float deltaTime, spine::Physics physics);
	void		 Render(FoxtrotRenderer* renderer, Transform* transform, Camera* camInst);

public:
	spine::Skeleton*	   GetSkeleton();
	spine::AnimationState* GetAnimState();
	float				   GetTimeScale();

	void SetTimeScale(float val);

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

private:
	spine::Skeleton*	   mSkeleton;
	spine::AnimationState* mState;
	float				   mTimeScale;
	size_t				   mMaxVertexCount;
	size_t				   mMaxIndexCount;
};
