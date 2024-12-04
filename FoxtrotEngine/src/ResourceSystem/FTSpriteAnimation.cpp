#include "ResourceSystem/FTSpriteAnimation.h"

#include "FileSystem/ChunkFileKeys.h"
#include "Renderer/FoxtrotRenderer.h"

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
	if (mReel[mCurrFrame]->Duration <= mAccTime)
	{
		++mCurrFrame;
		if (mMaxFrameIdx < mCurrFrame) // if maxIdx is 2, currFrame must be bigger than two
		{
			if (!mIsRepeated) 
			{
				mCurrFrame = 0;
				mIsFinished = true;
			}
			else
			{
				// Set current frame to the start.
				// (mMaxFrameIdx starts from 0, so the number of frames should be 
				//  mMaxFrameIdx + 1)
				mCurrFrame -= (mMaxFrameIdx + 1);
			}
		}
		mAccTime = 0.f;
	}
}

void FTSpriteAnimation::Render(ComPtr<ID3D11DeviceContext>& context)
{
	if (FrameIsWithinIndexRange())
		FTBasicMeshGroup::Render(context, mCurrFrame);
	LogInt("Frame : ", mCurrFrame);
}

std::string& FTSpriteAnimation::GetName()
{
	return mName;
}

AnimationFrame* FTSpriteAnimation::GetFrame()
{
	if (mIsFinished)
		LogString("Reel has finished playing");
	return mReel[mCurrFrame];
}

UINT FTSpriteAnimation::GetTexKey()
{
	return mTexKey;
}

UINT FTSpriteAnimation::GetTileMapKey()
{
	return mTileMapKey;
}

bool FTSpriteAnimation::GetIsFinished()
{
	return mIsFinished;
}

void FTSpriteAnimation::SetName(std::string&& name)
{
	mName.assign(name);
}

void FTSpriteAnimation::SetFrame(int frameNumber)
{
	mIsFinished = false;
	mCurrFrame = frameNumber;
	mAccTime = 0.f;
}

void FTSpriteAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh* mesh = GetMeshes()[frameNum];
	reinterpret_cast<AnimationFrame*>(mesh)->Duration = duration;
}

void FTSpriteAnimation::SetAnimator(AnimatorComponent* animator)
{
	mAnimator = animator;
}

void FTSpriteAnimation::SetIsFinished(bool val)
{
	mIsFinished = val;
}

void FTSpriteAnimation::SetIsRepeated(bool val)
{
	mIsRepeated = val;
}

void FTSpriteAnimation::SetTexKey(UINT key)
{
	mTexKey = key;
}

void FTSpriteAnimation::SetTileMapKey(UINT key)
{
	mTileMapKey = key;
}

void FTSpriteAnimation::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes)
{
	GetMeshes().reserve(meshes.size());
	mReel.reserve(meshes.size());
	for (const MeshData& meshData : meshes) {
		Mesh* newMesh = DBG_NEW Mesh;
		newMesh->mIndexCount = UINT(meshData.indices.size());
		newMesh->mVertexCount = UINT(meshData.vertices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
			newMesh->vertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.indices,
			newMesh->indexBuffer);

		newMesh->vertexConstantBuffer	= GetVertexConstantBuffer();
		newMesh->pixelConstantBuffer	= GetPixelConstantBuffer();

		this->GetMeshes().push_back(newMesh);

		AnimationFrame* frame = DBG_NEW AnimationFrame;
		frame->Duration = 1 / mAnimFPS;
		mReel.push_back(frame);
	}
}

FTSpriteAnimation::FTSpriteAnimation()
	: mName			{}
	, mMaxFrameIdx  (21)
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
	return 0 <= mCurrFrame && mCurrFrame <= mMaxFrameIdx;
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
void FTSpriteAnimation::UpdateMaxFrame()
{
	ImGui::InputInt("Max Frame", &mMaxFrameIdx);
}
#endif // FOXTROT_EDITOR