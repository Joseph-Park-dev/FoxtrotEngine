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

#ifdef FOXTROT_EDITOR
	#include "EditorResourceManager.h"
	#include <bitset>
#endif // FOXTROT_EDITOR

void FTSpineAnimation::InitializeSpinAnim(ComPtr<ID3D11Device>& device, spine::SkeletonData* skel)
{
	if (0 < mMeshes.size())
	{
		for (SpineMesh* mesh : mMeshes)
		{
			if (mesh)
			{
				delete mesh;
				mesh = nullptr;
			}
		}
		mMeshes.clear();
	}

	mSkeleton  = new spine::Skeleton(skel);
	mStateData = new spine::AnimationStateData(skel);
	mState	   = new spine::AnimationState(mStateData);

	//// Registers the clip inside of the Spine Animation.
	mLoadedClips.addAll(skel->getAnimations());

	if (mSkeletonData)
		if (0 < mSkeletonData->getSkins().size())
			mSkins.addAll(mSkeletonData->getSkins());
	SetSkin();

	auto drawOrder = mSkeleton->getDrawOrder();
	mMeshes.reserve(drawOrder.size());
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
	std::reverse(mMeshes.begin(), mMeshes.end());

	spine::Bone::setYDown(false);

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

void FTSpineAnimation::Render(FoxtrotRenderer* renderer)
{
	if (!GetVertexShader() || !GetPixelShader()) // Vertex Shader is always required when drawing.
		return;

	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
	UpdateBuffers(context);

	for (SpineMesh* mesh : mMeshes)
	{
		context->VSSetConstantBuffers(
			0, 1, mesh->VCBuf.GetAddressOf());

		if (GetTexture())
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(GetTexture()->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(GetVertexShader()->GetShader().Get(), 0, 0);

		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		context->PSSetShader(GetPixelShader()->GetShader().Get(), 0, 0);

		if (!Materials().empty())
		{
			context->PSSetConstantBuffers(
				0, 1, mesh->PCBuf.GetAddressOf());
		}

		FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		context->OMSetBlendState(renderer->GetBlendState().Get(), blendFactor, D3D11_DEFAULT_SAMPLE_MASK);

		context->IASetInputLayout(GetVertexShader()->GetInputLayout().Get());
		mesh->Draw(context);
	}
}

void FTSpineAnimation::SetJSONKey(FTDS::String& key)
{
	mJSONKey.Assign(key);
}

void FTSpineAnimation::SetAtlasKey(FTDS::String& key)
{
	mAtlasKey.Assign(key);
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

void FTSpineAnimation::SetMaterials(std::vector<FTDS::String>& matKeys, ComPtr<ID3D11Device>& device)
{
	if (0 < Materials().size())
		Materials().clear();

	if (matKeys.empty())
	{
		Debug::LogError(__LINE__, __FILE__, "Material Key not assigned");
		return;
	}

#ifdef FOXTROT_EDITOR
	if (1 < EditorResourceManager::GetInstance()->GetMaterials()->GetSize())
		for (FTDS::String& key : matKeys)
			Materials().push_back(EditorResourceManager::GetInstance()->GetLoadedMaterial(key));
#else
	if (1 < ResourceManager::GetInstance()->GetMaterials()->GetSize())
		for (FTDS::String& key : matKeys)
			Materials().push_back(ResourceManager::GetInstance()->GetLoadedMaterial(key));
#endif // FOXTROT_EDITOR

	for (SpineMesh* mesh : mMeshes)
	{
		for (FTMaterial* mat : Materials())
		{
			ComPtr<ID3D11Buffer> pixelConstBuffer;
			mat->CreatePixelConstBuffer(device, pixelConstBuffer);
			mesh->PCBuf = pixelConstBuffer;
		}
	}
}

void FTSpineAnimation::SetAnimation(int idx, bool loop)
{
	if (mCurrAnimIdx == idx)
		return;
	mState->setAnimation(0, mLoadedClips[idx], loop);
	mCurrAnimIdx = idx;
}

spine::Vector<spine::Animation*>& FTSpineAnimation::LoadedClips()
{
	return mLoadedClips;
}

FTSpineAnimation::FTSpineAnimation()
	: FTAnimation()
	, mJSONKey()
	, mAtlasKey()
	, mSkeletonData(nullptr)
	, mStateData(nullptr)
	, mSkeleton(nullptr)
	, mSkinCombination(0x0)
	, mCurrAnimIdx(-1)
	, mAtlas(nullptr)
	, mState(nullptr)
	, mTimeScale(1.f)
{
}

FTSpineAnimation::~FTSpineAnimation()
{
	delete mStateData;
	delete mState;
	delete mSkeletonData;
	delete mSkeleton->getSkin();
	delete mSkeleton;
	delete mAtlas;

	mStateData	  = nullptr;
	mState		  = nullptr;
	mSkeletonData = nullptr;
	mSkeleton	  = nullptr;
	mAtlas		  = nullptr;

	for (SpineMesh* mesh : mMeshes)
	{
		delete mesh;
		mesh = nullptr;
	}
}

void FTSpineAnimation::InitializeMeshes(
	ComPtr<ID3D11Device>&			 device,
	int								 order,
	void*							 attachment,
	SpineMesh::SPINE_ATTACHMENT_TYPE attachmentType)
{
	size_t vertexCount = 4;
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
	mMeshes.push_back(mesh);
}

void FTSpineAnimation::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	for (SpineMesh* mesh : mMeshes)
	{
		D3D11Utils::CreateConstantBuffer(device, GetVCData(), mesh->VCBuf);

		for (FTMaterial* mat : Materials())
		{
			ComPtr<ID3D11Buffer> pixelConstBuffer;
			mat->CreatePixelConstBuffer(device, pixelConstBuffer);
			mesh->PCBuf = pixelConstBuffer;
		}
	}
}

void FTSpineAnimation::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	for (SpineMesh* mesh : mMeshes)
	{
		D3D11Utils::UpdateBuffer(
			context, GetVCData(), mesh->VCBuf);

		size_t bufferCount = 0;
		if (!Materials().empty())
		{
			for (FTMaterial* mat : Materials())
			{
				mat->UpdateBuffer(context, mesh->PCBuf);
				++bufferCount;
			}
		}
	}
}

void FTSpineAnimation::UpdateBuffers(ComPtr<ID3D11DeviceContext>& context)
{
	if (!mSkeleton)
		return;

	for (SpineMesh* mesh : mMeshes)
	{
		spine::Slot*	   slot		  = mSkeleton->getDrawOrder()[mesh->DrawOrder];
		spine::Attachment* attachment = slot->getAttachment();
		if (mesh->MeshType == SpineMesh::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH)
		{
			spine::MeshAttachment* attm = static_cast<spine::MeshAttachment*>(attachment);

			// 텍스처
			spine::TextureRegion* texRegion = attm->getRegion();
			if (!texRegion)
				continue;

			auto*	   atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);
			FTTexture* texture	   = static_cast<FTTexture*>(atlasRegion->page->texture);
			if (!texture)
				continue;
			SetTexture(texture);

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

			// RegionAttachment → TextureRegion
			spine::TextureRegion* texRegion = region->getRegion();
			if (!texRegion)
				continue;

			// TextureRegion → AtlasRegion
			auto* atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);

			// AtlasPage → rendererObject
			auto* page	 = atlasRegion->page;
			auto* texSRV = reinterpret_cast<ID3D11ShaderResourceView*>(page->texture);
			if (!texSRV)
				continue;

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
	}
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

void FTSpineAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
	FTAnimation::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::JSON_KEY, mJSONKey);
	FileIOHelper::SaveString(ofs, ChunkKey::ATLAS_KEY, mAtlasKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::SKIN_COMBINATION, mSkinCombination);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_SPINE_ANIMATION_GROUP);
}

void FTSpineAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs);
	UINT skinCombi = 0;
	FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
	FileIOHelper::LoadBasicString(ifs, mAtlasKey);
	FileIOHelper::LoadBasicString(ifs, mJSONKey);
	FTAnimation::LoadProperties(ifs);

	mSkinCombination = static_cast<unsigned char>(skinCombi);
	std::bitset<8> bit(mSkinCombination);
}

void FTSpineAnimation::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	std::ifstream ifs(this->RelativePath().C_Str());
	this->LoadProperties(ifs);

	FTJSON* json = nullptr;
	FTText* text = nullptr;

#ifdef FOXTROT_EDITOR
	json = EditorResourceManager::GetInstance()->GetLoadedJSON(mJSONKey);
	text = EditorResourceManager::GetInstance()->GetLoadedText(mAtlasKey);
	EditorResourceManager::GetInstance()->RelativeToAbsolutePath(text);
#else
	json = ResourceManager::GetInstance()->GetLoadedJSON(mJSONKey);
	text = ResourceManager::GetInstance()->GetLoadedText(mAtlasKey);
	ResourceManager::GetInstance()->RelativeToAbsolutePath(text);
#endif // FOXTROT_EDITOR

	spine::FTSpineLoader* spineLoader = AnimationManager::GetInstance()->GetSpineLoader();
	mAtlas							  = new spine::Atlas(text->RelativePath().C_Str(), spineLoader);
	mSkeletonData					  = spineLoader->ReadSkeletonJsonData(json->RelativePath().C_Str(), mAtlas, 1.0f);
	InitializeSpinAnim(coreInst->GetGameRenderer()->GetDevice(), mSkeletonData);

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
#endif // FOXTROT_EDITOR