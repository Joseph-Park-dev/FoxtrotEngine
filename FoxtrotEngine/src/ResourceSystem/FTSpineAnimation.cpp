#include "ResourceSystem/FTSpineAnimation.h"

#include "spine/spine.h"

using namespace spine;

FTSpineAnimation::FTSpineAnimation()
	: mAtlasPath("E:/OneDrive/[02_Programming]/SpineRuntimeImp/TEST.atlas")
	, mJSONPath("E:/OneDrive/[02_Programming]/SpineRuntimeImp/TEST.json")
{
	mTexLoader = new MyTextureLoader;

	mAtlas = new Atlas(mAtlasPath, mTexLoader);
	if (mAtlas->getPages().size() == 0)
	{
		printf("Failed to load atlas");
		delete mAtlas;
		exit(0);
	}

	// Load the skeleton data
	SkeletonJson json(mAtlas);
	mSkeletonData = json.readSkeletonDataFile(mJSONPath);
	if (!mSkeletonData)
	{
		printf("Failed to load skeleton data");
		delete mAtlas;
		exit(0);
	}

	// Setup mix times
	mAnimStateData = new AnimationStateData(mSkeletonData);

	mSkeleton  = new Skeleton(mSkeletonData);
	mAnimState = new AnimationState(mAnimStateData);

	mskeletonRenderer = new SkeletonRenderer;
}

FTSpineAnimation::~FTSpineAnimation()
{
}

void FTSpineAnimation::Update(float deltaTime)
{
	// First update the animation state by the delta time
	mAnimState->update(deltaTime);

	// Next, apply the state to the skeleton
	mAnimState->apply(*mSkeleton);

	// Update the skeleton's frame time for physics
	mSkeleton->update(deltaTime);

	// Calculate world transforms for rendering
	mSkeleton->updateWorldTransform(spine::Physics_None);
}

void FTSpineAnimation::Render(FoxtrotRenderer* renderer)
{
	command = mskeletonRenderer->render(*mSkeleton);
	while (command)
	{
		spine::Vector<Vertex> vertices;
		Vertex				  vertex;
		float*				  positions = command->positions;
		float*				  uvs		= command->uvs;
		uint32_t*			  colors	= command->colors;
		uint16_t*			  indices	= command->indices;
		FTTexture*			  texture	= (FTTexture*)command->texture;
		for (int i = 0, j = 0, n = command->numVertices * 2; i < n; ++i, j += 2)
		{
			vertex.position.x = positions[j];
			vertex.position.y = positions[j + 1];
			vertex.texcoord.x = uvs[j];
			vertex.texcoord.y = uvs[j + 1];
			vertices.add(vertex);
			printf("x : %f, y : %f", vertex.position.x = positions[j], vertex.position.y = positions[j + 1]);
		}
		BlendMode blendMode = command->blendMode;

		vertices.clear();
		command = command->next;
	}
}

MyTextureLoader::MyTextureLoader()
	: spine::TextureLoader()
{
}