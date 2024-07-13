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
    InitializeMesh(GeometryGenerator::MakeSpineAnimation(mSkeletonRenderer->render(*mSkeleton)));
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
    spine::RenderCommand* command = mSkeletonRenderer->render(*mSkeleton);
    while (command) {
        MeshData meshData;
        Vertex vertex;
        float* positions = command->positions;
        float* uvs = command->uvs;
        uint32_t* colors = command->colors;
        uint16_t* indices = command->indices;
        SetTexture((FTTexture*)command->texture);
        for (int i = 0, j = 0, n = command->numVertices * 2; i < n; ++i, j += 2) {
            vertex.position.x = positions[j];
            vertex.position.y = positions[j + 1];
            vertex.position.z = 0.0f;
            vertex.texcoord.x = uvs[j];
            vertex.texcoord.y = uvs[j + 1];
            vertex.color.x = 1.0f;
            vertex.color.y = 1.0f;
            vertex.color.z = 1.0f;
            vertex.normal.x = 0.0f;
            vertex.normal.y = 0.0f;
            vertex.normal.z = -1.0f;
            meshData.vertices.push_back(vertex);
        }
        //renderer->UpdateBuffer(meshData.vertices, GetMesh()->vertexBuffer);
        //renderer->UpdateBuffer(meshData.indices, GetMesh()->indexBuffer);
        command = command->next;
        //smeshData.indices.clear();
    }
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
