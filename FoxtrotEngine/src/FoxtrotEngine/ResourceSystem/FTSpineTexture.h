#pragma once
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"

#include <spine/spine.h>
#include <spine/TextureLoader.h>

enum BlendMode {
    // See http://esotericsoftware.com/git/spine-runtimes/blob/spine-libgdx/spine-libgdx/src/com/esotericsoftware/spine/BlendMode.java#L37
    // for how these translate to OpenGL source/destination blend modes.
    BLEND_NORMAL,
    BLEND_ADDITIVE,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
};

class FTSpineTexture :
    public FTTexture {
public:
    // Initialization Phase
    void LoadSpineAsset(spine::String atlasFileName);
    void CreateSkeleton();

    // Gameloop Phase
    void Update(float deltaTime);
    void Render(FoxtrotRenderer* renderer);

public:
    spine::AtlasPage*       GetAtlasPage()      { return mAtlasPage; }
    spine::Atlas*           GetAtlas()          { return mAtlas; }
    spine::Skeleton*        GetSkeleton()       { return mSkeleton; }
    spine::SkeletonData*    GetSkeletonData()   { return mSkeletonData; }
    spine::AnimationState*  GetAnimationState() { return mAnimationState; }

public:
    FTSpineTexture()
        : FTTexture()
    {}
    ~FTSpineTexture() override;

private:
    spine::Atlas*          mAtlas;
    spine::AtlasPage*      mAtlasPage;
    spine::Skeleton*       mSkeleton;
    spine::SkeletonData*   mSkeletonData;
    spine::AnimationState* mAnimationState;

private:
    // Initialization Phase
    void LoadTextureAtlas(spine::String atlasFileN);
    void LoadSkeletonData(spine::String dataFileN);
    void InitAnimationStateData();
};