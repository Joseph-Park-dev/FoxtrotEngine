#pragma once
#include "Component.h"

#include <TemplateFunctions.h>
#include <ResourceManager.h>
#include <SpriteComponent.h>
#include <FTSpineTexture.h>

class SpineAnimComponent :
    public SpriteComponent
{
public:
    void SetTexture(spine::AtlasPage& page, const spine::String& path);

public:
    virtual void Initialize();
            void Initialize(spine::String atlasFileName);
    virtual void Update(float deltaTime)		   override;
    virtual void Render(FoxtrotRenderer* renderer) override;

public:
    SpineAnimComponent();

private:
    FoxtrotRenderer* mRenderer;
    FTSpineTexture*  mTexture;

private:
    mutable bool                          mOwnsAnimationStateData;
    mutable spine::Vector<float>          mWorldVertices;
    mutable spine::Vector<float>          mTempUvs;
    mutable spine::Vector<spine::Color>   mTempColors;
    mutable spine::Vector<unsigned short> mQuadIndices;
    mutable spine::SkeletonClipping       mClipper;
    mutable bool                          mUsePremultipliedAlpha;

private:
    void DrawSkeleton(FoxtrotRenderer* renderer);
};

//#include <spine/Extension.h>
//spine::SpineExtension* spine::getDefaultExtension() {
//    return new spine::DefaultSpineExtension();
//}