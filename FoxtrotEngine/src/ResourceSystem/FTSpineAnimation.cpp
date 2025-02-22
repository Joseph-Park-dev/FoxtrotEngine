#include "ResourceSystem/FTSpineAnimation.h"

#include "spine/spine.h"

#include "Renderer/FoxtrotRenderer.h"

using namespace spine;

FTSpineAnimation::FTSpineAnimation(FoxtrotRenderer* renderer)
	: mAtlasPath("E:/OneDrive/[02_Programming]/SpineRuntimeImp/TEST.atlas")
	, mJSONPath("E:/OneDrive/[02_Programming]/SpineRuntimeImp/TEST.json")
{
}

FTSpineAnimation::~FTSpineAnimation()
{
}

void FTSpineAnimation::Initialize(FoxtrotRenderer* renderer)
{
	CreateTextureSampler(renderer->GetDevice());

	mTexLoader = new MyTextureLoader(renderer);

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

	mAnimState->setAnimation(0, "animation", true);

	mskeletonRenderer = new SkeletonRenderer;

	Mesh*		   mesh	   = new Mesh;
	RenderCommand* command = mskeletonRenderer->render(*mSkeleton);

	//D3D11Utils::CreateVertexBuffer<Vertex>(renderer->GetDevice(), command->numVertices, mesh->VertexBuffer);
	//D3D11Utils::CreateIndexBuffer(renderer->GetDevice(), command->numIndices, mesh->IndexBuffer);
	//mesh->VertexCount = command->numVertices;
	//mesh->IndexCount  = command->numIndices;
	//D3D11Utils::CreateConstantBuffer(renderer->GetDevice(), mesh->VertexConstantData, mesh->VertexConstantBuffer);
	//D3D11Utils::CreateConstantBuffer(renderer->GetDevice(), mesh->PixelConstantData, mesh->PixelConstantBuffer);

	GetMeshes().push_back(mesh);
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
		MeshData meshData = MeshData();

		Vertex	   vertex;
		float*	   positions = command->positions;
		float*	   uvs		 = command->uvs;
		uint32_t*  colors	 = command->colors;
		uint16_t*  indices	 = command->indices;
		FTTexture* texture	 = (FTTexture*)command->texture;

		meshData.vertices.reserve(command->numVertices);
		meshData.indices.reserve(command->numIndices);

		for (int i = 0, j = 0; i < command->numVertices; i++, j += 2)
		{
			vertex.position.x = positions[j] / 150.f;
			vertex.position.y = positions[j + 1] / 150.f;
			vertex.texcoord.x = uvs[j];
			vertex.texcoord.y = 1 - uvs[j + 1];
			vertex.normal	  = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

			meshData.vertices.push_back(vertex);

			printf("x : %f, x : %f\n", vertex.position.x, vertex.position.y);
		}
		for (int i = 0; i < command->numIndices; i++)
			meshData.indices.push_back(static_cast<uint32_t>(indices[i]));
		// std::reverse(meshData.indices.begin(), meshData.indices.begin() + meshData.indices.size());

		Mesh* mesh		  = GetMeshes()[0];
		mesh->VertexCount = meshData.vertices.size();
		mesh->IndexCount  = meshData.indices.size();

		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.vertices.data(), mesh->VertexBuffer);
		D3D11Utils::UpdateBuffer(renderer->GetContext(), meshData.indices.data(), mesh->IndexBuffer);

		BlendMode blendMode = command->blendMode;

		FTBasicMeshGroup::Render(renderer, texture);
		command = command->next;
	}
}

MyTextureLoader::MyTextureLoader(FoxtrotRenderer* renderer)
	: spine::TextureLoader()
	, mrend(renderer)
{
}