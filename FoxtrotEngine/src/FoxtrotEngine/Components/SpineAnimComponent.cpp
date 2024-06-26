#include "SpineAnimComponent.h"

#include <DirectXTex.h>
#include <spine/spine.h>

#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/ResourceSystem/FTSpineAnimation.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"

void SpineAnimComponent::SetTexture(spine::AtlasPage& page, const spine::String& path)
{
    mTexture->SetRelativePath(std::string(path.buffer()));
    if (mTexture->GetRelativePath().empty()) {
        LogString("SpineTexture::CreateTexture() : Texture path empty,");
        return;
    }
        
}

void SpineAnimComponent::Initialize(spine::String atlasFileName)
{
    mRenderer = FTCore::GetInstance()->GetGameRenderer();
    mTexture = ResourceManager::GetInstance()->GetLoadedSpineTexture(atlasFileName);
    mTexture->LoadSpineAsset(atlasFileName);
    mTexture->CreateSkeleton();

    //mQuadIndices.add(0);
    //mQuadIndices.add(1);
    //mQuadIndices.add(2);
    //mQuadIndices.add(2);
    //mQuadIndices.add(3);
    //mQuadIndices.add(0);
}

void SpineAnimComponent::Update(float deltaTime)
{
    mTexture->Update(deltaTime);
    SpriteComponent::Update(deltaTime);
}

void SpineAnimComponent::Render(FoxtrotRenderer* renderer)
{
    DrawSkeleton(renderer);
    SpriteComponent::Render(renderer);
}

SpineAnimComponent::SpineAnimComponent()
    : SpriteComponent(GetOwner(), DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER)
{
    Initialize();
    SpriteComponent::Initialize(mRenderer);
}

void SpineAnimComponent::DrawSkeleton(FoxtrotRenderer* renderer){
    // For each slot in the draw order array of the skeleton
    spine::Skeleton* skeleton = mTexture->GetSkeleton();
    for (size_t i = 0, n = skeleton->getSlots().size(); i < n; ++i) {
        spine::Slot* slot = skeleton->getDrawOrder()[i];

        // Fetch the currently active attachment, continue
        // with the next slot in the draw order if no
        // attachment is active on the slot
        spine::Attachment* attachment = slot->getAttachment();
        if (!attachment) continue;

        // Fetch the blend mode from the slot and
        // translate it to the engine blend mode
        BlendMode engineBlendMode;
        switch (slot->getData().getBlendMode()) {
            case spine::BlendMode_Normal:
                engineBlendMode = BlendMode::BLEND_NORMAL;
                break;
            case spine::BlendMode_Additive:
                engineBlendMode = BlendMode::BLEND_ADDITIVE;
                break;
            case spine::BlendMode_Multiply:
                engineBlendMode = BlendMode::BLEND_MULTIPLY;
                break;
            case spine::BlendMode_Screen:
                engineBlendMode = BlendMode::BLEND_SCREEN;
                break;
            default:
                // unknown Spine blend mode, fall back to
                // normal blend mode
                engineBlendMode = BlendMode::BLEND_NORMAL;
        }

        // Calculate the tinting color based on the skeleton's color
        // and the slot's color. Each color channel is given in the
        // range [0-1], you may have to multiply by 255 and cast to
        // and int if your engine uses integer ranges for color channels.
        spine::Color skeletonColor = skeleton->getColor();
        spine::Color slotColor = slot->getColor();
        spine::Color tint(
            skeletonColor.r * slotColor.r,
            skeletonColor.g * slotColor.g,
            skeletonColor.b * slotColor.b,
            skeletonColor.a * slotColor.a
        );

        spine::Vector<float> worldVertices;
        spine::Vector<float> uvs;
        spine::Vector<unsigned short> indices;
        size_t indicesCount = GetMeshData().indices.size();
        worldVertices.setSize(8, 0.f);
        for (size_t i = 0; i < GetMeshData().vertices.size(); ++i) {
            worldVertices[i] = GetMeshData().vertices[i].position.x;
            worldVertices[i] = GetMeshData().vertices[i].position.y;
        }
        for (size_t i = 0; i < indicesCount; ++i) {
            indices.add(GetMeshData().indices[i]);
        }
        // Fill the vertices array, indices, and texture depending on the type of attachment
        spine::Color* attachmentColor;

        if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
            spine::RegionAttachment* regionAttachment = (spine::RegionAttachment*)attachment;
            attachmentColor = &regionAttachment->getColor();

            // Early out if the slot color is 0
            if (attachmentColor->a == 0) {
                mClipper.clipEnd(*slot);
                continue;
            }
            regionAttachment->computeWorldVertices(*slot, worldVertices, 0, 2);
            uvs = regionAttachment->getUVs();
            mTexture = (FTSpineAnimation*)((spine::AtlasRegion*)regionAttachment->getRegion())->page->texture;
        }
        else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
            spine::MeshAttachment* mesh = (spine::MeshAttachment*)attachment;
            attachmentColor = &mesh->getColor();

            // Early out if the slot color is 0
            if (attachmentColor->a == 0) {
                mClipper.clipEnd(*slot);
                continue;
            }

            worldVertices.setSize(mesh->getWorldVerticesLength(), 0);
            mesh->computeWorldVertices(*slot, 0, mesh->getWorldVerticesLength(), worldVertices.buffer(), 0, 2);
            uvs = mesh->getUVs();
            indices = mesh->getTriangles();
            indicesCount = mesh->getTriangles().size();
            mTexture = (FTSpineAnimation*)((spine::AtlasRegion*)mesh->getRegion())->page->texture;
        }
        else if (attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti)) {
            spine::ClippingAttachment* clip = (spine::ClippingAttachment*)slot->getAttachment();
            mClipper.clipStart(*slot, clip);
            continue;
        }
        else
            continue;
        renderer->
    }
}
