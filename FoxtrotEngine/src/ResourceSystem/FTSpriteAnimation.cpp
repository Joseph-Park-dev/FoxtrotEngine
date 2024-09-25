#include "ResourceSystem/FTSpriteAnimation.h"

void FTSpriteAnimation::Initialize(std::vector<MeshData>& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	FTBasicMeshGroup::InitializeConstantBuffer(device);
	this->InitializeMeshes(device, meshes);
	FTBasicMeshGroup::CreateShaders(device);
}

void FTSpriteAnimation::Update(float deltaTime)
{
	if (mIsFinished)
		return;
	mAccTime += deltaTime;
	if (mReel[mCurrFrame]->Duration < mAccTime)
	{
		++mCurrFrame;
		if (mReel.size() <= mCurrFrame)
		{
			if (!mIsRepeated) {
				mCurrFrame = -1;
				mIsFinished = true;
				mAccTime = 0.f;
				return;
			}
		}
		mAccTime -= mReel[mCurrFrame]->Duration;
	}
}

void FTSpriteAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh* mesh = GetMeshes()[frameNum];
	reinterpret_cast<AnimationFrame*>(mesh)->Duration = duration;
}

void FTSpriteAnimation::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes)
{
	for (const MeshData& meshData : meshes) {
		AnimationFrame* newMesh = new AnimationFrame;
		newMesh->mIndexCount = UINT(meshData.indices.size());
		newMesh->mVertexCount = UINT(meshData.vertices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
			newMesh->vertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.indices,
			newMesh->indexBuffer);

		newMesh->vertexConstantBuffer	= GetVertexConstantBuffer();
		newMesh->pixelConstantBuffer	= GetPixelConstantBuffer();

		this->GetMeshes().push_back(newMesh);
	}
}

FTSpriteAnimation::FTSpriteAnimation()
	: mName			{}
	, mAnimFPS		(60.0f)
	, mIsRepeated	(true)
	, mAnimator		(nullptr)
	, mReel			{}
	, mCurrFrame	(0)
	, mAccTime		(0.f)
	, mIsFinished	(false)

{}