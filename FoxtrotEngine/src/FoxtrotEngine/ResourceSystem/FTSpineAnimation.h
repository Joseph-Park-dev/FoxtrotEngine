#pragma once
#include "FoxtrotEngine/ResourceSystem/FTResource.h"

#include "spine/spine.h"

class SpineTextureLoader;

class FTSpineAnimation :
    public FTResource{
public:
	bool CreateAtlas(std::string atlasPath, SpineTextureLoader* spineTexLoaderInst);
	bool CreateSkeletonDataBinary(std::string skeletonDataPath);
	bool CreateSkeletonDataJSON(std::string skeletonDataPath);
	bool CreateAnimationStateData();

public:
	spine::SkeletonData*		GetSkeletonData()		{ return mSkeletonData; }
	spine::AnimationStateData*	GetAnimationStateData() { return mAnimationStateData; }

private:
    spine::Atlas*				mAtlas;
	spine::SkeletonData*		mSkeletonData;
	spine::AnimationStateData*	mAnimationStateData;

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;
};