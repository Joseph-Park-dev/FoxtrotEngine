#pragma once
#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "spine/Extension.h"
#include "spine/spine.h"
#include "spine/TextureLoader.h"

#include "FoxtrotEngine/ResourceSystem/FTSpineAnimation.h"

class SpineAnimRendererComponent :
    public MeshRendererComponent
{
public:
    // core paramter
    // Editor Mode : FTCoreEditor
    // Game Mode   : FTCore
    virtual	void Initialize(FTCore* coreInstance)	override;
    virtual void Update(float deltaTime)			override;
    virtual void LateUpdate(float deltaTime)		override {};
    virtual void Render(FoxtrotRenderer* renderer)  override;

public:
    virtual std::wstring GetName() const override { return L"SpineAnimRendererComponent"; }

private:
    FTSpineAnimation*        mSpineAnimation;
    spine::Skeleton*         mSkeleton;
    spine::AnimationState*   mAnimationState;

    spine::SkeletonRenderer* mSkeletonRenderer;
    float                    mScale;

public:
    SpineAnimRendererComponent(Actor* owner, int drawOrder, int updateOrder);
    ~SpineAnimRendererComponent() override;

private:
    void RenderSkeleton(FoxtrotRenderer* renderer);

private:
    void AddTest();

#ifdef _DEBUG
    //This section will be omitted from Release mode!
public:
    //virtual void SaveProperties(std::ofstream& ofs) override;
    //virtual void LoadProperties(std::ifstream& ifs) override;

    virtual void EditorUpdate(float deltaTime) override;
    virtual void EditorUIUpdate() override;
#endif // DEBUG
};

spine::SpineExtension* spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}