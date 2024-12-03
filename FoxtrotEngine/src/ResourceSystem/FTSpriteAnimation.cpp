#include "ResourceSystem/FTSpriteAnimation.h"

#include "FileSystem/ChunkFileKeys.h"
#include "Renderer/FoxtrotRenderer.h"

void FTSpriteAnimation::Initialize(std::vector<MeshData>& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	FTBasicMeshGroup::InitializeConstantBuffer(device);
	this->InitializeMeshes(device, meshes);
	FTBasicMeshGroup::CreateShaders(device);
	mMaxFrame = meshes.size();
}

void FTSpriteAnimation::Update(float deltaTime)
{
	if (mIsFinished)
		return;
	if (0 < mMaxFrame)
	{
		mCurrFrame += mAnimFPS * deltaTime;
		while (mCurrFrame >= mMaxFrame)
		{
			if (mIsRepeated)
				mCurrFrame -= mMaxFrame;
			else {
				mCurrFrame = 0;
				break;
			}
		}
	}
	LogInt(static_cast<int>(mCurrFrame));
}

void FTSpriteAnimation::Render(ComPtr<ID3D11DeviceContext>& context)
{
	if (FrameIsWithinIndexRange())
		FTBasicMeshGroup::Render(context, static_cast<int>(mCurrFrame));
}

void FTSpriteAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh* mesh = GetMeshes()[frameNum];
	reinterpret_cast<AnimationFrame*>(mesh)->Duration = duration;
}

void FTSpriteAnimation::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes)
{
	GetMeshes().reserve(meshes.size());
	for (const MeshData& meshData : meshes) {
		AnimationFrame* newMesh = DBG_NEW AnimationFrame;
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
	, mMaxFrame		(0.f)
	, mAnimFPS		(30.0f)
	, mIsRepeated	(true)
	, mAnimator		(nullptr)
	, mReel			{}
	, mCurrFrame	(0)
	, mAccTime		(0.f)
	, mIsFinished	(false)

	, mTexKey		(VALUE_NOT_ASSIGNED)
	, mTileMapKey	(VALUE_NOT_ASSIGNED)
{}

FTSpriteAnimation::~FTSpriteAnimation()
{
	for (size_t i = 0; i < mReel.size(); ++i)
	{
		if (mReel[i])
		{
			delete mReel[i];
			mReel[i] = nullptr;
		}
	}
	mReel.clear();
}

bool FTSpriteAnimation::FrameIsWithinIndexRange()
{
	return 0 <= mCurrFrame && mCurrFrame < mMaxFrame;
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs, UINT key)
{
}

UINT FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	return 0;
}

#ifdef FOXTROT_EDITOR
void FTSpriteAnimation::UpdateUI()
{
	UpdateIsRepeated();
}

void FTSpriteAnimation::UpdateIsRepeated()
{
	ImGui::Checkbox("Is Repeated", &mIsRepeated);
}

void FTSpriteAnimation::OnConfirmUpdate()
{
}
#endif // FOXTROT_EDITOR