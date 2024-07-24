#include "FoxtrotEngine/Components/SpineAnimRendererComponent.h"

#include "spine/spine.h"

#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/Renderer/GeometryGenerator.h"

void SpineAnimRendererComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
    SetKey("RunningGrandpa");
    mSpineAnimation = ResourceManager::GetInstance()->GetLoadedSpineAnimation(GetKey());
    mSkeleton = new spine::Skeleton(mSpineAnimation->GetSkeletonData());
    mSkeleton->setX(GetOwner()->GetTransform()->GetWorldPosition().x);
    mSkeleton->setY(GetOwner()->GetTransform()->GetWorldPosition().y);

    mSpineAnimation->GetAnimationStateData()->setDefaultMix(0.5f);
    mAnimationState = new spine::AnimationState(mSpineAnimation->GetAnimationStateData());
    mAnimationState->setAnimation(0, "Idling", false);

    mSkeletonRenderer = new spine::SkeletonRenderer();
    InitializeMesh(GeometryGenerator::MakeSpineMeshes(*mSkeletonRenderer, *mSkeleton));
    //SetTexture("./assets/Asteroid.png");
}

void SpineAnimRendererComponent::Update(float deltaTime)
{
    // First update the animation state by the delta time
    mAnimationState->update(deltaTime);

    // Next, apply the state to the skeleton
    mAnimationState->apply(*mSkeleton);

    // Update the skeleton's frame time for physics
    mSkeleton->update(deltaTime);

    // Calculate world transforms for rendering
    mSkeleton->updateWorldTransform(spine::Physics_None);

	MeshRendererComponent::Update(deltaTime);
}

void SpineAnimRendererComponent::Render(FoxtrotRenderer* renderer)
{
    RenderSkeleton(renderer);
	MeshRendererComponent::Render(renderer);
}

SpineAnimRendererComponent::SpineAnimRendererComponent(Actor* owner, int drawOrder, int updateOrder)
    : MeshRendererComponent(owner, drawOrder, updateOrder)
{
}

SpineAnimRendererComponent::~SpineAnimRendererComponent()
{
    if (mSpineAnimation)
        delete mSpineAnimation;
    if (mSkeleton)
        delete mSkeleton;
    if (mSkeletonRenderer)
        delete mSkeletonRenderer;
}

void SpineAnimRendererComponent::RenderSkeleton(FoxtrotRenderer* renderer)
{
    //smeshData.indices.clear();
}

void SpineAnimRendererComponent::AddTest()
{
}

void SpineAnimRendererComponent::EditorUpdate(float deltaTime)
{
    Update(deltaTime);
}

void SpineAnimRendererComponent::EditorUIUpdate()
{
    if (ImGui::Button("Add Test")) {
        AddTest();
    }
}
