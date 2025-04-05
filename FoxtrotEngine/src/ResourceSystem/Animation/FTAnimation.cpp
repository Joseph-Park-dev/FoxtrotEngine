#include "FTAnimation.h"

#include "ResourceSystem/Animation/AnimationFrame.h"

bool		FTAnimation::GetIsFinished() const { return mIsFinished; }
int			FTAnimation::GetCurrFrameIdx() const { return mCurrFrameIdx; }

void FTAnimation::SetFrame(int frameNumber)
{
	mIsFinished	  = false;
	mCurrFrameIdx = frameNumber;
	mAccTime	  = 0.f;
}

void FTAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh*			mesh  = Meshes().at(frameNum);
	AnimationFrame* frame = static_cast<AnimationFrame*>(mesh);
	frame->Duration		  = duration;
}

void FTAnimation::SetAnimator(Animator* animator) { mAnimator = animator; }
void FTAnimation::SetIsFinished(bool val) { mIsFinished = val; }
void FTAnimation::SetIsRepeated(bool val) { mIsRepeated = val; }

void FTAnimation::IncreaseIdx() { ++mCurrFrameIdx; }

void FTAnimation::Update(float deltaTime)
{
	Mesh*			mesh  = Meshes().at(mCurrFrameIdx);
	AnimationFrame* frame = static_cast<AnimationFrame*>(mesh);
	UpdateFrame(deltaTime, frame->Duration);
}

void FTAnimation::Render(FoxtrotRenderer* renderer)
{
	if (FrameIsWithinIndexRange(mCurrFrameIdx))
		FTBasicMeshGroup::Render(renderer, mCurrFrameIdx);
}

FTAnimation::FTAnimation()
	: FTBasicMeshGroup()
	, mType(AnimationType::NOT_ASSIGNED)
	, mMaxFrameIdx(0)
	, mAnimFPS(30.0f)
	, mIsRepeated(false)
	, mAnimator(nullptr)
	, mCurrFrameIdx(0)
	, mAccTime(0.f)
	, mIsFinished(false)
{
}

FTAnimation::FTAnimation(FTAnimation* other)
	: FTBasicMeshGroup()
	, mType(other->mType)
	, mMaxFrameIdx(other->mMaxFrameIdx)
	, mAnimFPS(other->mAnimFPS)
	, mIsRepeated(other->mIsRepeated)
	, mAnimator(other->mAnimator)
	, mCurrFrameIdx(0)
	, mAccTime(0.f)
	, mIsFinished(false)
{
}

void FTAnimation::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes)
{
	if (0 < meshes.size())
		Clear();

	Meshes().reserve(meshes.size());
	for (const FTMeshData& meshData : meshes)
	{
		AnimationFrame* newFrame = DBG_NEW AnimationFrame;
		newFrame->VertexCount	 = UINT(meshData.Vertices.size());
		newFrame->IndexCount	 = UINT(meshData.Indices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, newFrame->VertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.Indices, newFrame->IndexBuffer);

		newFrame->Duration = 1 / mAnimFPS;
		this->Meshes().push_back(newFrame);
	}

	SetNormalLines(DBG_NEW Mesh);
	std::vector<Vertex>	  normalVertices;
	std::vector<uint32_t> normalIndices;

	size_t offset = 0;
	for (const FTMeshData& meshData : meshes)
	{
		for (size_t i = 0; i < meshData.Vertices.size(); ++i)
		{
			Vertex v = meshData.Vertices.at(i);

			v.texcoord.x = 0.0f; // start point
			normalVertices.push_back(v);

			v.texcoord.x = 1.0f; // end point
			normalVertices.push_back(v);

			normalIndices.push_back(uint32_t(2 * (i + offset)));
			normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		}
		offset = meshData.Vertices.size();
	}

	D3D11Utils::CreateVertexBuffer<Vertex>(device, normalVertices, NormalLines()->VertexBuffer);
	NormalLines()->VertexCount = (UINT)normalVertices.size();
	D3D11Utils::CreateIndexBuffer(device, normalIndices, NormalLines()->IndexBuffer);
	NormalLines()->IndexCount = (UINT)normalIndices.size();
}

void FTAnimation::UpdateFrame(float deltaTime, float frameDuration)
{
	if (mIsFinished)
		return;
	mAccTime += deltaTime;
	if (frameDuration <= mAccTime)
	{
		++mCurrFrameIdx;
		if (mMaxFrameIdx < mCurrFrameIdx) // if maxIdx is 2, currFrame must be bigger than two
		{
			if (!mIsRepeated)
			{
				mCurrFrameIdx = 0;
				mIsFinished	  = true;
			}
			else
			{
				// Set current frame to the start.
				// (mMaxFrameIdx starts from 0, so the number of frames should be
				//  mMaxFrameIdx + 1)
				mCurrFrameIdx -= (mMaxFrameIdx + 1);
			}
		}
		mAccTime = 0.f;
	}
}

bool FTAnimation::FrameIsWithinIndexRange(int currentFrame)
{
	return 0 <= currentFrame && currentFrame <= mMaxFrameIdx;
}

void FTAnimation::SaveProperties(std::ofstream& ofs, UINT key)
{
	FTBasicMeshGroup::SaveProperties(ofs, key);
	FileIOHelper::SaveInt(ofs, ChunkKey::Animation::TYPE, static_cast<int>(mType));
	FileIOHelper::SaveFloat(ofs, ChunkKey::Animation::FPS, mAnimFPS);
	FileIOHelper::SaveBool(ofs, ChunkKey::Animation::IS_REPEATED, mIsRepeated);
	FileIOHelper::SaveInt(ofs, ChunkKey::Animation::MAX_FRAME_IDX, mMaxFrameIdx);
}

UINT FTAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mMaxFrameIdx);
	FileIOHelper::LoadBool(ifs, mIsRepeated);
	FileIOHelper::LoadFloat(ifs, mAnimFPS);
	
	int type = 0;
	FileIOHelper::LoadInt(ifs, type);
	mType = static_cast<AnimationType>(type);

	return FTBasicMeshGroup::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTAnimation::UpdateUI()
{
	UpdateIsRepeated();
	UpdateMaxFrame();
}

void FTAnimation::UpdateIsRepeated()
{
	ImGui::Checkbox("Is Repeated", &mIsRepeated);
}

void FTAnimation::UpdateMaxFrame()
{
	ImGui::InputInt("Max Frame", &mMaxFrameIdx);
}
#endif // FOXTROT_EDITOR