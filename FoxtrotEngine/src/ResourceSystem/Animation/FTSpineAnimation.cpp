#include "ResourceSystem/Animation/FTSpineAnimation.h"

#include <spine/spine.h>

#include "Managers/AnimationManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/FTMeshData.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/GenericData/FTText.h"

#ifdef FOXTROT_EDITOR
	#include "EditorResourceManager.h"
	#include <bitset>
#endif // FOXTROT_EDITOR

void FTSpineAnimation::InitializeSpinAnim(ComPtr<ID3D11Device>& device)
{
	if (0 < mMeshes->GetSize())
	{
		mMeshes->IterateArray([&](SpineMesh* mesh) {
			if (mesh)
			{
				delete mesh;
				mesh = nullptr;
			}
		});
		mMeshes->Clear();
	}

	spine::FTSpineLoader* spineLoader = AnimationManager::GetInstance()->GetSpineLoader();

	ResourceManager::GetInstance()->RelativeToAbsolutePath(mAtlasTxt);
	ResourceManager::GetInstance()->RelativeToAbsolutePath(mJSON);

	mAtlas = new spine::Atlas(mAtlasTxt->RelativePath().C_Str(), spineLoader);
	mSkeletonData = spineLoader->ReadSkeletonJsonData(mJSON->RelativePath().C_Str(), mAtlas, 1.0f);

	mStateData = new spine::AnimationStateData(mSkeletonData);
	mSkeleton  = new spine::Skeleton(mSkeletonData);
	mState	   = new spine::AnimationState(mStateData);

	//// Registers the clip inside of the Spine Animation.
	mLoadedClips.addAll(mSkeletonData->getAnimations());

	if (mSkeletonData)
		if (0 < mSkeletonData->getSkins().size())
			mSkins.addAll(mSkeletonData->getSkins());
	SetSkin();

	auto drawOrder = mSkeleton->getDrawOrder();
	mMeshes->Reserve(drawOrder.size());
	for (size_t i = 0; i < drawOrder.size(); ++i)
	{
		spine::Slot*	   slot		  = drawOrder[i];
		spine::Attachment* attachment = slot->getAttachment();
		if (!attachment)
			continue;
		if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			InitializeMeshes(device, i, attachment, SpineMesh::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH);
		}
		else if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			InitializeMeshes(device, i, attachment, SpineMesh::SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION);
		}
	}
	mMeshes->Reverse();

	// Set default properties.
	spine::Bone::setYDown(false);

	SetTimeScale(1);
	// drawable.setUsePremultipliedAlpha(true);

	mSkeleton->setToSetupPose();
	mSkeleton->setPosition(0.f, 0.f);
	mSkeleton->updateWorldTransform(spine::Physics_None);

	CreateTextureSampler(device);
	InitializeConstantBuffers(device);
}

void FTSpineAnimation::Update(float deltaTime, spine::Physics physics)
{
	if (!mState || !mSkeleton)
		return;

	mState->update(deltaTime * mTimeScale);
	mState->apply(*mSkeleton);
	mSkeleton->update(deltaTime * mTimeScale);
	mSkeleton->updateWorldTransform(physics);
}

void FTSpineAnimation::Render(
	FoxtrotRenderer* renderer,
	FTTexture*		 tex,
	FTVertexShader*	 vs,
	FTPixelShader*	 ps,
	FTMaterial*		 mat)
{
	if (!vs || !ps) // Vertex Shader is always required when drawing.
		return;

	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
	UpdateBuffers(context);

	mMeshes->IterateArray([&](SpineMesh* mesh) {
		context->VSSetConstantBuffers(
			0, 1, GetVCBuf().GetAddressOf());

		if (tex)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(tex->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);

		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		context->PSSetShader(ps->GetShader().Get(), 0, 0);

		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		context->OMSetBlendState(renderer->GetBlendState().Get(), blendFactor, D3D11_DEFAULT_SAMPLE_MASK);

		context->IASetInputLayout(vs->GetInputLayout().Get());
		mesh->Draw(context);
	});
}

spine::Skeleton* FTSpineAnimation::GetSkeleton() const
{
	return mSkeleton;
}

spine::AnimationState* FTSpineAnimation::GetAnimState() const
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

void FTSpineAnimation::SetAnimation(int idx, bool loop)
{
	if (mCurrAnimIdx == idx)
		return;
	mCurrAnimIdx = idx;

	mState->setAnimation(0, mLoadedClips[idx], loop);
}

spine::Vector<spine::Animation*>& FTSpineAnimation::LoadedClips()
{
	return mLoadedClips;
}

FTSpineAnimation::FTSpineAnimation()
	: FTAnimation()
	, mStateData(nullptr)
	, mSkinCombination(0x0)
	, mCurrAnimIdx(-1)
	, mTimeScale(1.f)
	, mMeshes(DBG_NEW FTDS::DynamicArray<SpineMesh*>)
{
}

FTSpineAnimation::~FTSpineAnimation()
{
	if (mSkeleton)
	{
		delete mSkeleton->getSkin();
		delete mSkeleton;
	}

	if (mAtlas)
		delete mAtlas;

	if (mSkeletonData)
		delete mSkeletonData;

	if (mState)
		delete mState;

	if (mStateData)
		delete mStateData;

	mStateData	  = nullptr;
	mState		  = nullptr;
	mSkeleton	  = nullptr;

	mMeshes->IterateArray([&](SpineMesh* mesh) {
		delete mesh;
		mesh = nullptr;
	});

	delete mMeshes;
}

void FTSpineAnimation::InitializeMeshes(
	ComPtr<ID3D11Device>&			 device,
	int								 order,
	void*							 attachment,
	SpineMesh::SPINE_ATTACHMENT_TYPE attachmentType)
{
	size_t vertexCount = 0;
	UINT   indexCount  = 0;

	if (SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH == attachmentType)
	{
		auto* mesh	= static_cast<spine::MeshAttachment*>(attachment);
		vertexCount = mesh->getWorldVerticesLength() / 2;
		indexCount	= mesh->getTriangles().size();
	}
	else if (SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION == attachmentType)
	{
		vertexCount = 4;
		indexCount	= 6;
	}

	SpineMesh* mesh = DBG_NEW SpineMesh;
	mesh->ResourceBinding(device, order, attachmentType, vertexCount, indexCount);
	mMeshes->PushBack(mesh);
}

void FTSpineAnimation::UpdateConstantBuffers(
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context,
	FTMaterial*					 mat)
{
	mMeshes->IterateArray([&](SpineMesh* mesh) {
		D3D11Utils::UpdateBuffer(
			context, GetVCData(), GetVCBuf());
	});

	if (mat)
		mat->UpdateBuffer(context);
}

void FTSpineAnimation::UpdateBuffers(ComPtr<ID3D11DeviceContext>& context)
{
	if (!mSkeleton)
		return;

	mMeshes->IterateArray([&](SpineMesh* mesh) {
		if (!mesh)
			return;

		spine::Slot*	   slot		  = mSkeleton->getDrawOrder()[mesh->DrawOrder];
		spine::Attachment* attachment = slot->getAttachment();
		if (mesh->MeshType == SpineMesh::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH)
		{
			spine::MeshAttachment* attm = static_cast<spine::MeshAttachment*>(attachment);

			// 텍스처
			spine::TextureRegion* texRegion = attm->getRegion();
			if (!texRegion)
				return;

			auto*	   atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);
			FTTexture* texture	   = static_cast<FTTexture*>(atlasRegion->page->texture);
			if (!texture)
				return;

			// 위치 변환 및 복사
			{
				size_t					 bufSize = attm->getWorldVerticesLength();
				D3D11_MAPPED_SUBRESOURCE mapped	 = {};
				if (SUCCEEDED(context->Map(mesh->PositionBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					float* ptr = (float*)mapped.pData;
					attm->computeWorldVertices(*slot, 0, bufSize, ptr, 0, 2);
					context->Unmap(mesh->PositionBuf.Get(), 0);
				}
			}

			// Copy Texture Coordinates
			{
				const float* uvs	  = attm->getUVs().buffer(); // 총 x, y 요소를 담은 배열
				auto		 eleCount = attm->getUVs().size();	 // 총 x, y 요소의 개수

				// 버퍼로 데이터를 복사합니다.
				D3D11_MAPPED_SUBRESOURCE mapped = {};
				if (SUCCEEDED(context->Map(mesh->TexcoordBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					memcpy(mapped.pData, uvs, sizeof(float) * eleCount);

					// memcpy_s(&mapped.pData, sizeof(float) * eleCount, uvs, sizeof(float) * eleCount);
					context->Unmap(mesh->TexcoordBuf.Get(), 0);
				}
			}
			// index buffer
			{
				const uint16_t*			 indices	= attm->getTriangles().buffer();
				UINT					 indexCount = attm->getTriangles().size();
				D3D11_MAPPED_SUBRESOURCE mapped		= {};
				if (SUCCEEDED(context->Map(mesh->IndexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					memcpy(mapped.pData, indices, sizeof(uint16_t) * indexCount);
					context->Unmap(mesh->IndexBuf.Get(), 0);
				}
			}
		}
		else if (mesh->MeshType == SpineMesh::SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION)
		{
			auto* region = static_cast<spine::RegionAttachment*>(attachment);

			// RegionAttachment -> TextureRegion
			spine::TextureRegion* texRegion = region->getRegion();
			if (!texRegion)
				return;

			// TextureRegion → AtlasRegion
			auto* atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);

			// AtlasPage → rendererObject
			auto* page	 = atlasRegion->page;
			auto* texSRV = reinterpret_cast<ID3D11ShaderResourceView*>(page->texture);
			if (!texSRV)
				return;

			// 8. 렌더링 정점 복사
			// 위치 변환 및 복사
			{
				size_t					 vtxCount = 8;
				D3D11_MAPPED_SUBRESOURCE mapped	  = {};
				if (SUCCEEDED(context->Map(mesh->PositionBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					float* ptr = (float*)mapped.pData;
					region->computeWorldVertices(*slot, ptr, 0, 2);
					context->Unmap(mesh->PositionBuf.Get(), 0);
				}
			}

			// Copy Texture Coordinates
			{
				const float* uvs	  = region->getUVs().buffer(); // 총 x, y 요소를 담은 배열
				auto		 eleCount = region->getUVs().size();   // 총 x, y 요소의 개수

				// 버퍼로 데이터를 복사합니다.
				D3D11_MAPPED_SUBRESOURCE mapped = {};
				if (SUCCEEDED(context->Map(mesh->TexcoordBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					memcpy(mapped.pData, uvs, sizeof(float) * eleCount);

					// memcpy_s(&mapped.pData, sizeof(float) * eleCount, uvs, sizeof(float) * eleCount);
					context->Unmap(mesh->TexcoordBuf.Get(), 0);
				}
			}

			// index buffer
			{
				const uint16_t indices[6] = {
					0, 1, 2, 0, 2, 3, // 앞면
				};
				UINT					 indexCount = 6;
				D3D11_MAPPED_SUBRESOURCE mapped		= {};
				if (SUCCEEDED(context->Map(mesh->IndexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
				{
					memcpy(mapped.pData, indices, sizeof(uint16_t) * indexCount);
					context->Unmap(mesh->IndexBuf.Get(), 0);
				}
			}
		}
	});
}

void FTSpineAnimation::SetSkin()
{
	spine::Skin* prev = mSkeleton->getSkin();
	spine::Skin* skin = new spine::Skin("Skin");

	// Loop through bitmask, marking if 1.
	int bitCount = sizeof(mSkinCombination) * 8; // Total number of bits in the integer
	for (int i = bitCount - 1; i >= 0; --i)
	{
		unsigned int mask = 1U << i; // Create a mask with a single bit at position 'i'
		if (mSkinCombination & mask)
		{ // Check if the bit at position 'i' is set
			skin->addSkin(mSkins[i]);
		}
	}
	mSkeleton->setSkin(skin);
	mSkeleton->setSlotsToSetupPose();

	delete prev;
}

void FTSpineAnimation::ToggleSkin(size_t idx)
{
	mSkinCombination ^= (1 << idx);
}

void FTSpineAnimation::SetJSON(FTJSON* json)
{
	mJSON = json;
}

void FTSpineAnimation::SetAtlasTxt(FTText* txt)
{
	mAtlasTxt = txt;
}

void FTSpineAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
	FTAnimation::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::JSON_KEY, mJSON->FileName());
	FileIOHelper::SaveString(ofs, ChunkKey::ATLAS_KEY, mAtlasTxt->FileName());
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::SKIN_COMBINATION, mSkinCombination);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
}

void FTSpineAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs);
	UINT skinCombi = 0;
	FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
	FileIOHelper::LoadResource(ifs, mAtlasTxt, ResourceManager::GetInstance()->GetTexts());
	FileIOHelper::LoadResource(ifs, mJSON, ResourceManager::GetInstance()->GetJSONs());
	FTAnimation::LoadProperties(ifs);

	mSkinCombination = static_cast<unsigned char>(skinCombi);
}

void FTSpineAnimation::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	std::ifstream ifs(this->RelativePath().C_Str());
	this->LoadProperties(ifs);

	FoxtrotRenderer* renderer = coreInst->GetGameRenderer();
	InitializeSpinAnim(renderer->GetDevice());

	this->SetIsProcessed(true);
}

#ifdef FOXTROT_EDITOR
void FTSpineAnimation::UpdateUI()
{
	static bool val[MAX_SKIN_COUNT];
	for (size_t i = 0; i < mSkins.size(); ++i)
	{
		CommandHistory::GetInstance()->UpdateBoolValue(mSkins[i]->getName().buffer(), val[i]);
		if (val[i])
			mSkinCombination |= (1 << i);
		else
			mSkinCombination &= ~(1 << i);
	}

	if (ImGui::Button("UpdateSkin"))
		SetSkin();
}

void FTSpineAnimation::AddRefCount()
{
	if (mJSON)
		mJSON->AddRefCount();

	if (mAtlasTxt)
		mAtlasTxt->AddRefCount();

	FTBasicMeshGroup::AddRefCount();
}

void FTSpineAnimation::SubtractRefCount()
{
	if (mJSON)
		mJSON->SubtractRefCount();

	if (mAtlasTxt)
		mAtlasTxt->SubtractRefCount();

	FTBasicMeshGroup::SubtractRefCount();
}
#endif // FOXTROT_EDITOR