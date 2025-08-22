#include "FTAnimation.h"

#include "ResourceSystem/Animation/AnimationFrame.h"
#include "Managers/AnimationManager.h"
#include "ResourceSystem/FTMeshData.h"

int FTAnimation::GetMaxFrameIdx() const { return mMaxFrameIdx; }
int FTAnimation::GetMinFrameIdx() const { return mMinFrameIdx; }

void FTAnimation::SetFrameDuration(int frameNum, float duration)
{
	Mesh*			mesh  = Meshes()->At(frameNum);
	AnimationFrame* frame = static_cast<AnimationFrame*>(mesh);
	frame->Duration		  = duration;
}

FTAnimation::FTAnimation()
	: FTBasicMeshGroup()
	, mType(AnimationType::NOT_ASSIGNED)
	, mMinFrameIdx(0)
	, mMaxFrameIdx(0)
	, mAnimFPS(30.0f)
	, mIsRepeated(true)
{
}

FTAnimation::FTAnimation(FTAnimation* other)
	: FTBasicMeshGroup()
	, mType(other->mType)
	, mMinFrameIdx(other->mMinFrameIdx)
	, mMaxFrameIdx(other->mMaxFrameIdx)
	, mAnimFPS(other->mAnimFPS)
	, mIsRepeated(other->mIsRepeated)
{
}

void FTAnimation::Initialize(FTDS::DynamicArray<FTMeshData*>&& meshData, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	if (0 < Meshes()->GetSize())
		Clear();

	Meshes()->Reserve(meshData.GetSize());
	meshData.IterateArray([&](FTMeshData* meshD) {

		if (!meshD->Vertices.IsEmpty() && !meshD->Indices.IsEmpty())
		{
			AnimationFrame* newFrame = DBG_NEW AnimationFrame;
			newFrame->VertexCount	 = UINT(meshD->Vertices.GetSize());
			newFrame->IndexCount	 = UINT(meshD->Indices.GetSize());

			D3D11Utils::CreateVertexBuffer(device, meshD->Vertices, newFrame->VertexBuffer);
			D3D11Utils::CreateIndexBuffer(device, meshD->Indices, newFrame->IndexBuffer);

			newFrame->Duration = 1 / mAnimFPS;
			Meshes()->PushBack(newFrame);
			delete meshD;
		}
	});

	if (mMaxFrameIdx == 0)
		mMaxFrameIdx = meshData.GetSize() - 1;

	CreateTextureSampler(device);
	InitializeConstantBuffers(device);
}

void FTAnimation::SaveProperties(std::ofstream& ofs)
{
	FTBasicMeshGroup::SaveProperties(ofs);
	FileIOHelper::SaveInt(ofs, ChunkKey::Animation::TYPE, static_cast<int>(mType));
	FileIOHelper::SaveFloat(ofs, ChunkKey::Animation::FPS, mAnimFPS);
	FileIOHelper::SaveBool(ofs, ChunkKey::Animation::IS_REPEATED, mIsRepeated);
	FileIOHelper::SaveInt(ofs, ChunkKey::Animation::MAX_FRAME_IDX, mMaxFrameIdx);
	FileIOHelper::SaveInt(ofs, ChunkKey::Animation::MIN_FRAME_IDX, mMinFrameIdx);
}

void FTAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mMinFrameIdx);
	FileIOHelper::LoadInt(ifs, mMaxFrameIdx);
	FileIOHelper::LoadBool(ifs, mIsRepeated);
	FileIOHelper::LoadFloat(ifs, mAnimFPS);

	int type = 0;
	FileIOHelper::LoadInt(ifs, type);
	mType = static_cast<AnimationType>(type);

	FTBasicMeshGroup::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTAnimation::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateIntValue("Min Frame", mMinFrameIdx);
	CommandHistory::GetInstance()->UpdateIntValue("Max Frame", mMaxFrameIdx);
}
#endif // FOXTROT_EDITOR