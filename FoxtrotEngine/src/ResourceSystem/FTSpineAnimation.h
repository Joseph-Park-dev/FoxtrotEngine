#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

#include <spine/spine.h>

#include "Managers/ResourceManager.h"

class MyTextureLoader : public spine::TextureLoader
{
public:
	MyTextureLoader();

	virtual ~MyTextureLoader() {}

	// Called when the atlas loads the texture of a page.
	virtual void load(spine::AtlasPage& page, const spine::String& path)
	{
		FTTexture* texture = ResourceManager::GetInstance()->GetLoadedTexture(path.buffer());

		// if texture loading failed, we simply return.
		if (!texture)
			return;

		// store the Texture on the rendererObject so we can
		// retrieve it later for rendering.
		page.texture = texture;
	}

	// Called when the atlas is disposed and itself disposes its atlas pages.
	virtual void unload(void* texture) override
	{
		printf("Unloaded");
	}
};

class FTSpineAnimation
{
public:
	FTSpineAnimation();
	~FTSpineAnimation();

public:
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