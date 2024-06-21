#include "FoxtrotEngine/ResourceSystem/FTSpineTexture.h"

#include <spine/spine.h>

#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

void FTSpineTexture::LoadSpineAsset(spine::String atlasFileName){
    //SetRelativePath(atlasFileName.buffer());
    LoadTextureAtlas(atlasFileName);
    LoadSkeletonData(atlasFileName);
    InitAnimationStateData();
}

void FTSpineTexture::CreateSkeleton(){
    spine::SkeletonBinary binary(mAtlas);
    binary.setScale(2);

    // Load the skeleton .skel file into a SkeletonData
    spine::String path = static_cast<spine::String>(GetRelativePath().c_str());
    mSkeletonData = binary.readSkeletonDataFile(path);

    // If loading failed, print the error and exit the app
    if (!mSkeletonData) {
        printf("%s\n", binary.getError().buffer());
        exit(0);
    }

    mSkeleton = new spine::Skeleton(mSkeletonData);
    mSkeleton->setX(0.f);
    mSkeleton->setY(0.f);

    // Create the animation state and enqueue a random animation, looping
    //mAnimationState->setAnimation(0, animationNames[random(0, 3)], true);
}

void FTSpineTexture::Update(float deltaTime){
    mAnimationState->update(deltaTime);
    mAnimationState->apply(*mSkeleton);
    mSkeleton->updateWorldTransform(spine::Physics_None);
}

void FTSpineTexture::Render(FoxtrotRenderer* renderer){
    
}

FTSpineTexture::~FTSpineTexture()
{
	delete mAtlas;
}

void FTSpineTexture::LoadTextureAtlas(spine::String atlasFileN)
{
    mAtlas = new spine::Atlas(atlasFileN, ResourceManager::GetInstance());
    if (mAtlas->getPages().size() == 0) {
        printf("Failed to load atlas");
        delete mAtlas;
        return;
    }
}

void FTSpineTexture::LoadSkeletonData(spine::String dataFileN)
{
    spine::SkeletonBinary skeletonBin(mAtlas);
    mSkeletonData = skeletonBin.readSkeletonDataFile(dataFileN);
    if (!mSkeletonData) {
        printf("Error: FTSpineTexture:: LoadSkeletonData() -> Failed to load skeleton data");
        delete mAtlas;
        return;
    }
}

void FTSpineTexture::InitAnimationStateData()
{
    spine::AnimationStateData* animationStateData = new spine::AnimationStateData(mSkeletonData);
    // Animation State data value modification here
    // ex) setmix

    mAnimationState = new spine::AnimationState(animationStateData);
    // Set initial aniamtion state here

    delete animationStateData;
}