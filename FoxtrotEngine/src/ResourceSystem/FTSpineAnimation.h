#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

#include <spine/spine.h>

#include "Managers/ResourceManager.h"
#include "Renderer/FoxtrotRenderer.h"

class MyTextureLoader : public spine::TextureLoader
{
public:
	MyTextureLoader(FoxtrotRenderer* renderer);

	virtual ~MyTextureLoader() {}

	// Called when the atlas loads the texture of a page.
	virtual void load(spine::AtlasPage& page, const spine::String& path)
	{
		FTTexture* texture = new FTTexture();
		texture->SetFileName("test");
		texture->SetRelativePath(path.buffer());

		D3D11Utils::CreateTexture(mrend->GetDevice(), mrend->GetContext(), texture);

		// if texture loading failed, we simply return.
		if (!texture)
			return;

		// store the Texture on the rendererObject so we can
		// retrieve it later for rendering.
		page.texture = (void*)texture;
	}

	// Called when the atlas is disposed and itself disposes its atlas pages.
	virtual void unload(void* texture) override
	{
		printf("Unloaded");
	}

private:
	FoxtrotRenderer* mrend;
};

class FTSpineAnimation : public FTBasicMeshGroup
{
public:
	FTSpineAnimation(FoxtrotRenderer* renderer);
	~FTSpineAnimation();

public:
	void Initialize(FoxtrotRenderer* renderer);
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);

private:
	// Setup Pose Data (To be moved to ResourceManager)
	spine::String			   mAtlasPath;
	spine::String			   mJSONPath;
	spine::Atlas*			   mAtlas;
	spine::AnimationStateData* mAnimStateData;
	spine::SkeletonData*	   mSkeletonData;

	// InstanceData
	spine::Skeleton*	   mSkeleton;
	spine::AnimationState* mAnimState;
	spine::RenderCommand*  command;

	// New Manager
	MyTextureLoader*		 mTexLoader;
	spine::SkeletonRenderer* mskeletonRenderer;
};