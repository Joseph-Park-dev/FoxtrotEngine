#include "ResourceSystem/FTSpineAnimation.h"

#include "Managers/ResourceManager.h"

bool FTSpineAnimation::CreateAtlas(std::string atlasPath, SpineTextureLoader* spineTexLoaderInst){
	if (mAtlas)
		delete mAtlas;
	mAtlas = new spine::Atlas(atlasPath.c_str(), spineTexLoaderInst);
	if (mAtlas)
		return true;
	return false;
}

bool FTSpineAnimation::CreateSkeletonDataBinary(std::string skeletonDataPath)
{
	if (mAtlas) {
		spine::SkeletonBinary binary(mAtlas);
		binary.setScale(1.0f);
		mSkeletonData = binary.readSkeletonDataFile(skeletonDataPath.c_str());
		if (!mSkeletonData) {
			printf("%s\n", binary.getError().buffer());
			return false;
		}
		else {
			return true;
		}
		return false;
	}
	return false;
}

bool FTSpineAnimation::CreateSkeletonDataJSON(std::string skeletonDataPath)
{
	if (mAtlas) {
		spine::SkeletonJson JSON(mAtlas);
		JSON.setScale(0.01f);
		mSkeletonData = JSON.readSkeletonDataFile(skeletonDataPath.c_str());
		if (!mSkeletonData) {
			printf("%s\n", JSON.getError().buffer());
			return false;
		}
		else {
			return true;
		}
		return false;
	}
	return false;
}

bool FTSpineAnimation::CreateAnimationStateData()
{
	mAnimationStateData = new spine::AnimationStateData(mSkeletonData);
	//mAnimationStateData->setDefaultMix(0.1f);
	return mAnimationStateData;
}

FTSpineAnimation::FTSpineAnimation()
	: mAtlas(nullptr)
	, mSkeletonData(nullptr)
	, mAnimationStateData(nullptr)
{
}

FTSpineAnimation::~FTSpineAnimation()
{
	if (mAtlas)
		delete mAtlas;
	if (mSkeletonData)
		delete mSkeletonData;
	if (mAnimationStateData)
		delete mAnimationStateData;
}