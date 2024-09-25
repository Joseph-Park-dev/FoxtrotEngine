#include "Components/SpineAnimRendererComponent.h"

#include "spine/spine.h"

#include "Managers/ResourceManager.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/Vertex.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/GeometryGenerator.h"

void SpineAnimRendererComponent::Initialize(FTCore* coreInstance)
{
	//MeshRendererComponent::Initialize(coreInstance);
 //   SetKey("RunningGrandpa");
 //   mSpineAnimation = ResourceManager::GetInstance()->GetLoadedSpineAnimation(GetKey());
 //   mSkeleton = new spine::Skeleton(mSpineAnimation->GetSkeletonData());
 //   mSkeleton->setX(GetOwner()->GetTransform()->GetWorldPosition().x);
 //   mSkeleton->setY(GetOwner()->GetTransform()->GetWorldPosition().y);
 //   mSkeleton->setSkin(NULL);

 //   mSpineAnimation->GetAnimationStateData()->setDefaultMix(0.5f);
 //   mAnimationState = new spine::AnimationState(mSpineAnimation->GetAnimationStateData());
 //   mAnimationState->setAnimation(0, "Idling", true);

 //   //InitializeMesh(GeometryGenerator::MakeSpineMeshes(mSkeletonRenderer, mSkeleton));
 //   //SetTexture("./assets/Asteroid.png");
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

void SpineAnimRendererComponent::Render(FoxtrotRenderer* renderer){
    std::vector<MeshData> meshes;
    spine::RenderCommand* command = mSkeletonRenderer->render(*mSkeleton);
    while (command) {
        MeshData meshData;
        FTTexture* tex = (FTTexture*)command->texture;
        float* positions = command->positions;
        float* uvs = command->uvs;
        uint32_t* colors = command->colors;
        uint16_t* indices = command->indices;
        for (int i = 0; i < command->numVertices << 1; i += 2) {
            Vertex vertex;
            vertex.position.x = positions[i];
            vertex.position.y = positions[i + 1];
            vertex.position.z = 0.0f;
            vertex.texcoord.x = uvs[i];
            vertex.texcoord.y = uvs[i + 1];
            vertex.color.x = 1.0f;
            vertex.color.y = 1.0f;
            vertex.color.z = 1.0f;
            vertex.normal.x = 0.0f;
            vertex.normal.y = 0.0f;
            vertex.normal.z = -1.0f;
            meshData.vertices.push_back(vertex);
        }
        for (size_t i = 0; i < command->numIndices; ++i) {
            meshData.indices.push_back(indices[i]);
        }
        meshes.push_back(meshData);
        //SetTexture((FTTexture*)command->texture);
        //InitializeMesh(std::move(meshes));
        MeshRendererComponent::Render(renderer);
        command = command->next;
    }
    
}

SpineAnimRendererComponent::SpineAnimRendererComponent(Actor* owner, int drawOrder, int updateOrder)
    : MeshRendererComponent(owner, drawOrder, updateOrder)
    , mSkeletonRenderer (new spine::SkeletonRenderer())
    , mSpineAnimation   (nullptr)
    , mSkeleton         (nullptr)
    , mAnimationState   (nullptr)
    , mScale            (0.01f)
{
}

SpineAnimRendererComponent::~SpineAnimRendererComponent()
{
    if (mSpineAnimation)
        delete mSpineAnimation;
    if (mSkeleton)
        delete mSkeleton;
    //if (mSkeletonRenderer)
    //    delete mSkeletonRenderer;
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
