#include "SpineAnimator.h"

#include "spine/spine.h"

#include "Managers/AnimationManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Core/FTCore.h"

void SpineAnimator::Initialize(FTCore* coreInst)
{
	SetRenderer(coreInst->GetGameRenderer());

	spine::FTSpineLoader loader;

	std::string	  path		= "E:\\OneDrive\\[02_Programming]\\SpineRuntimeImp\\export\\TEST.json";
	std::string	  atlasPath = "E:\\OneDrive\\[02_Programming]\\SpineRuntimeImp\\export\\TEST.atlas";
	spine::Atlas* atlas		= new spine::Atlas(atlasPath.c_str(), &loader);

	spine::SkeletonData* skeletonData = loader.ReadSkeletonJsonData(path.c_str(), atlas, 1.0f);

	spine::SkeletonRenderer* skeletonRenderer =
		AnimationManager::GetInstance()->GetSkeletonRenderer();

	// Create a skeleton from the data, set the skeleton's position to the bottom center of
	// the screen and scale it to make it smaller.
	mSkeleton = new spine::Skeleton(skeletonData);
	mSkeleton->setPosition(0.f, 0.f);

	spine::RenderCommand* command = skeletonRenderer->render(*mSkeleton);

	std::vector<FTMeshData> meshDataVec;
	size_t					i = 0;
	UINT texKey = 0;

	bool loadedTex = false;
	while (command)
	{
		if (!loadedTex)
		{
			//texKey = ResourceManager::GetInstance()->GetKey(
			//	(FTTexture*)command->texture,
			//	ResourceManager::GetInstance()->GetTexturesMap());
			//SetTexKey(texKey);
			//loadedTex = true;
		}

		FTMeshData meshData;
		InitMeshData(meshData, command);
		meshDataVec.push_back(meshData);

		++i;
		command = command->next;
	}

	SetMeshGroup(DBG_NEW FTBasicMeshGroup);
	GetMeshGroup()->Initialize(
		std::move(meshDataVec),
		coreInst->GetGameRenderer()->GetDevice(),
		coreInst->GetGameRenderer()->GetContext());

	GetMeshGroup()->SetTexture(texKey);

	// Create an AnimationState to drive animations on the skeleton. Set the "portal" animation
	// on track with index 0.
	spine::AnimationStateData animationStateData(skeletonData);
	animationStateData.setDefaultMix(0.2f);
	mState = new spine::AnimationState(&animationStateData);
	mState->setAnimation(0, "animation", true);

	// Create the renderer and set the viewport size to match the window size. This sets up a
	// pixel perfect orthogonal projection for 2D rendering.
	delete atlas;
}

void SpineAnimator::Update(float deltaTime)
{
	mState->update(deltaTime * mTimeScale);
	mState->apply(*mSkeleton);
	mSkeleton->update(deltaTime * mTimeScale);
	mSkeleton->updateWorldTransform(mUsePhysics);
}

void SpineAnimator::Render(FoxtrotRenderer* renderer)
{
	spine::SkeletonRenderer* skeletonRenderer =
		AnimationManager::GetInstance()->GetSkeletonRenderer();

	spine::RenderCommand* command = skeletonRenderer->render(*mSkeleton);

	size_t i = 0;
	while (command)
	{
		FTMeshData meshData;
		InitMeshData(meshData, command);

		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.Vertices, GetMeshGroup()->Meshes().at(i)->VertexBuffer);
		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.Indices, GetMeshGroup()->Meshes().at(i)->IndexBuffer);

		++i;
		command = command->next;
	}

	MeshRenderer::Render(renderer);
}

SpineAnimator::SpineAnimator(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mSkeleton(nullptr)
	, mState(nullptr)
	, mTimeScale(1.f)
	, mUsePhysics(spine::Physics_Update)
{
}

SpineAnimator::~SpineAnimator()
{
	delete mSkeleton;
	delete mState;
}

void SpineAnimator::CloneTo(Actor* actor)
{
	SpineAnimator* newComp = DBG_NEW SpineAnimator(actor, GetUpdateOrder());
}

void SpineAnimator::InitMeshData(FTMeshData& meshData, spine::RenderCommand* command)
{
	meshData.Vertices.reserve(command->numVertices);
	meshData.Indices.reserve(command->numIndices);

	float*	   positions = command->positions;
	float*	   uvs		 = command->uvs;
	uint32_t*  colors	 = command->colors;
	uint16_t*  indices	 = command->indices;
	FTTexture* texture	 = (FTTexture*)command->texture;
	FTVector2  size		 = texture->GetTexScale();

	for (int i = 0, j = 0; i < command->numVertices; i++, j += 2) {
		Vertex vertex;
		vertex.position.x = positions[j];
		vertex.position.y = positions[j + 1];
		vertex.texcoord.x = uvs[j];
		vertex.texcoord.y = uvs[j + 1];
		vertex.normal = Vector3(0.f, 0.f, -1.f);

		printf("%f   %f   \n", vertex.texcoord.x, vertex.texcoord.y);
		meshData.Vertices.push_back(vertex);
	}

	for (int i = 0; i < command->numIndices; ++i)
		meshData.Indices.push_back(indices[i]);
	//std::reverse(meshData.Indices.begin(), meshData.Indices.end());
}

#ifdef FOXTROT_EDITOR
void SpineAnimator::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}
#endif