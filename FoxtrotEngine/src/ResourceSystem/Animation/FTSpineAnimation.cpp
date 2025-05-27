#include "FTSpineAnimation.h"

#include "Managers/AnimationManager.h"
#include "Renderer/FoxtrotRenderer.h"

void FTSpineAnimation::Initialize(spine::SkeletonData* skel, spine::AnimationStateData* stateData)
{
	spine::Bone::setYDown(true);
	mSkeleton = new (__FILE__, __LINE__) spine::Skeleton(skel);
	stateData = new (__FILE__, __LINE__) spine::AnimationStateData(skel);
	mState	  = new (__FILE__, __LINE__) spine::AnimationState(stateData);
}

void FTSpineAnimation::Update(float deltaTime, spine::Physics physics)
{
	mState->update(deltaTime * mTimeScale);
	mState->apply(*mSkeleton);
	mSkeleton->update(deltaTime * mTimeScale);
	mSkeleton->updateWorldTransform(physics);
}

void FTSpineAnimation::Render(FoxtrotRenderer* renderer, Transform* transform, Camera* camInst)
{
	SetTexture(nullptr);
	// vertexArray->clear();

	spine::SkeletonRenderer* skelRenderer = AnimationManager::GetInstance()->GetSkeletonRenderer();
	if (!skelRenderer)
		skelRenderer = new (__FILE__, __LINE__) spine::SkeletonRenderer();
	spine::RenderCommand* command = skelRenderer->render(*mSkeleton);
	size_t				  i		  = 0;
	while (command)
	{
		FTMeshData meshData;
		Vertex	   vertex;
		float*	   positions = command->positions;
		float*	   uvs		 = command->uvs;
		uint32_t*  colors	 = command->colors;
		uint16_t*  indices	 = command->indices;
		FTTexture* texture	 = (FTTexture*)command->texture;
		// Vector2u size = texture->getSize();
		for (int i = 0, n = command->numIndices; i < n; ++i)
		{
			int ii			  = indices[i];
			int index		  = ii << 1;
			vertex.position.x = positions[index];
			vertex.position.y = positions[index + 1];
			vertex.texcoord.x = uvs[index];
			vertex.texcoord.y = uvs[index + 1];
			vertex.normal	  = Vector3(0.f, 0.f, -1.f);
			// toSFMLColor(colors[ii], &vertex.color);
			meshData.Vertices.push_back(vertex);
			meshData.Indices.push_back(index);
		}
		// BlendMode blendMode = command->blendMode;
		// states.blendMode = usePremultipliedAlpha ? blendModesPma[blendMode] : blendModes[blendMode];
		// states.texture = texture;
		SetTexture(texture);
		if (mMaxVertexCount < meshData.Vertices.size())
			FTBasicMeshGroup::Initialize({ meshData }, renderer->GetDevice(), renderer->GetContext());

		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.Vertices.data(), Meshes().at(i)->VertexBuffer);
		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.Indices.data(), Meshes().at(i)->IndexBuffer);
		this->CalcVCData(transform, camInst);
		this->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

		FTAnimation::Render(renderer);
		// vertexArray->clear();

		command = command->next;
		++i;
	}
}

spine::Skeleton* FTSpineAnimation::GetSkeleton()
{
	return mSkeleton;
}

spine::AnimationState* FTSpineAnimation::GetAnimState()
{
	return mState;
}

float FTSpineAnimation::GetTimeScale()
{
	return mTimeScale;
}

void FTSpineAnimation::SetTimeScale(float val)
{
	mTimeScale = val;
}

FTSpineAnimation::FTSpineAnimation()
	: FTAnimation()
	, mSkeleton(nullptr)
	, mState(nullptr)
	, mTimeScale(1.f)
	, mMaxVertexCount(0)
	, mMaxIndexCount(0)
{
}

FTSpineAnimation::~FTSpineAnimation()
{
	delete mState;
	delete mSkeleton;
}