#include "ResourceSystem/Animation/FTSpineAnimation.h"

#include <spine/spine.h>
#include <wrl.h>
#include <d3d11.h>

#include "Manager/AnimationManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/FTSpineLoader.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/FTTexture.h"

#ifdef FOXTROT_EDITOR
	#include <bitset>
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using Microsoft::WRL::ComPtr;

	void FTSpineAnimation::Update(float deltaTime, spine::Physics physics)
	{
		if (!mState || !mSkeleton)
			return;

		mState->update(deltaTime * mTimeScale);
		mState->apply(*mSkeleton);
		mSkeleton->update(deltaTime * mTimeScale);
		mSkeleton->updateWorldTransform(physics);
	}

	void FTSpineAnimation::Render(D3D11Renderer* renderer, Core::Transform* transform, Camera* camInst, FTTexture* tex, FTVertexShader* vs, FTPixelShader* ps, FTMaterial* mat)
	{
		// This enables the resource reusable throughout the Component instances.
		UpdateConstantBuffers(renderer, transform, camInst, mat, GetFrontDir());

		if (!vs || !ps || !mat) // Vertex Shader is always required when drawing.
			return;

		ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
		UpdateBuffers(context);

		mMeshes->IterateArray([&](SpineMesh* mesh) {
			context->VSSetConstantBuffers(
				0, 1, GetVCBuf().GetAddressOf());

			if (tex)
			{
				std::vector<ID3D11ShaderResourceView*> resViews;
				resViews.push_back(tex->GetSRV().Get());
				context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
			}

			context->VSSetShader(vs->GetShader().Get(), 0, 0);

			context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
			context->PSSetShader(ps->GetShader().Get(), 0, 0);

			if (mat)
				context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

			context->IASetInputLayout(vs->GetInputLayout().Get());
			mesh->Draw(context);
		});
	}

	void FTSpineAnimation::SetTimeScale(const float val)
	{
		mTimeScale = val;
	}

	void FTSpineAnimation::SetAnimation(const int idx, const bool isRepeated)
	{
		if (mCurrAnimIdx == idx)
			return;
		mCurrAnimIdx = idx;

		mState->setAnimation(0, mLoadedClips[idx], isRepeated);
	}

	void FTSpineAnimation::ToggleSkin(const size_t idx)
	{
		mSkinCombination ^= (1 << idx);
	}

	spine::Vector<spine::Animation*>& FTSpineAnimation::LoadedClips()
	{
		return mLoadedClips;
	}

	FTSpineAnimation::FTSpineAnimation(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTSprite(resDef, renderer)
		, mJSONPath(nullptr)
		, mAtlasPath(nullptr)
		, mTimeScale(1.f)
		, mSkinCombination(0x0)
		, mCurrAnimIdx(-1)
		, mAtlas(nullptr)
		, mSkeletonData(nullptr)
		, mSkeleton(nullptr)
		, mStateData(nullptr)
		, mState(nullptr)
		, mMeshes(DBG_NEW Common::FTDS::DynamicArray<SpineMesh*>)
	{
		Process(renderer);
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

		mStateData = nullptr;
		mState	   = nullptr;
		mSkeleton  = nullptr;

		mMeshes->IterateArray([&](SpineMesh* mesh) {
			delete mesh;
			mesh = nullptr;
		});

		delete mMeshes;
	}

	void FTSpineAnimation::Process(D3D11::D3D11Renderer* renderer)
	{
		if (mJSONPath && mAtlasPath && !mMeshes->IsEmpty())
			return;

		std::ifstream ifs(GetFileName()->C_Str());

		// If the file exists (not being created).
		if (ifs.good())
			this->LoadProperties(ifs);

		// The game will attempt to reload the resource after this call.
		if (!mJSONPath || !mAtlasPath)
			return;

		InitializeSpinAnim(renderer->GetDevice());
	}

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

		FTSpineLoader* spineLoader = AnimationManager::GetInstance()->GetSpineLoader();

		// ResourceManager::GetInstance()->RelativeToAbsolutePath(mAtlasTxt);
		// ResourceManager::GetInstance()->RelativeToAbsolutePath(mJSON);

		mAtlas		  = new spine::Atlas(mAtlasPath->C_Str(), spineLoader);
		mSkeletonData = spineLoader->ReadSkeletonJsonData(mJSONPath->C_Str(), mAtlas, 1.0f);

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
				InitializeMeshes(device, static_cast<int>(i), attachment, D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH);
			}
			else if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
			{
				InitializeMeshes(device, static_cast<int>(i), attachment, D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION);
			}
		}

#ifdef FOXTROT_EDITOR
		mMeshes->Reverse();
#endif // FOXTROT_EDITOR

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

	void FTSpineAnimation::InitializeMeshes(
		ComPtr<ID3D11Device>&		 device,
		int							 order,
		void*						 attachment,
		D3D11::SPINE_ATTACHMENT_TYPE attachmentType)
	{
		size_t vertexCount = 0;
		size_t indexCount  = 0;

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

	void FTSpineAnimation::UpdateBuffers(ComPtr<ID3D11DeviceContext>& context)
	{
		if (!mSkeleton)
			return;

		mMeshes->IterateArray([&](SpineMesh* mesh) {
			if (!mesh)
				return;

			spine::Slot*	   slot		  = mSkeleton->getDrawOrder()[mesh->DrawOrder];
			spine::Attachment* attachment = slot->getAttachment();

			// For spine mesh attachment
			if (mesh->MeshType == D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH)
			{
				spine::MeshAttachment* attm = static_cast<spine::MeshAttachment*>(attachment);

				//// Check if the texture is present.
				// spine::TextureRegion* texRegion = attm->getRegion();
				// if (!texRegion)
				//	return;

				// auto*	   atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);
				// FTTexture* texture	   = static_cast<FTTexture*>(atlasRegion->page->texture);
				// if (!texture)
				//	return;

				// Copies the vertex positions.
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

				// Copies Texture Coordinates
				{
					const float* uvs	  = attm->getUVs().buffer();
					auto		 eleCount = attm->getUVs().size(); // The number of (x,y) coordinates.

					D3D11_MAPPED_SUBRESOURCE mapped = {};
					if (SUCCEEDED(context->Map(mesh->TexcoordBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
					{
						memcpy(mapped.pData, uvs, sizeof(float) * eleCount);
						context->Unmap(mesh->TexcoordBuf.Get(), 0);
					}
				}
				// Copies indices.
				{
					const uint16_t*			 indices	= attm->getTriangles().buffer();
					UINT					 indexCount = static_cast<UINT>(attm->getTriangles().size());
					D3D11_MAPPED_SUBRESOURCE mapped		= {};
					if (SUCCEEDED(context->Map(mesh->IndexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
					{
						memcpy(mapped.pData, indices, sizeof(uint16_t) * indexCount);
						context->Unmap(mesh->IndexBuf.Get(), 0);
					}
				}
			}
			// For spine mesh region, which is a square.
			else if (mesh->MeshType == D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION)
			{
				auto* region = static_cast<spine::RegionAttachment*>(attachment);

				// RegionAttachment -> TextureRegion
				spine::TextureRegion* texRegion = region->getRegion();
				if (!texRegion)
					return;

				// TextureRegion �� AtlasRegion
				auto* atlasRegion = reinterpret_cast<spine::AtlasRegion*>(texRegion);

				// AtlasPage �� rendererObject
				auto* page	 = atlasRegion->page;
				auto* texSRV = reinterpret_cast<ID3D11ShaderResourceView*>(page->texture);
				if (!texSRV)
					return;

				// Copies the vertex positions.
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

				// Copies Texture Coordinates
				{
					const float* uvs	  = region->getUVs().buffer();
					auto		 eleCount = region->getUVs().size(); // The number of (x,y) coordinates.

					D3D11_MAPPED_SUBRESOURCE mapped = {};
					if (SUCCEEDED(context->Map(mesh->TexcoordBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
					{
						memcpy(mapped.pData, uvs, sizeof(float) * eleCount);
						context->Unmap(mesh->TexcoordBuf.Get(), 0);
					}
				}

				// Copies indices.
				{
					const uint16_t indices[6] = {
						0,
						1,
						2,
						0,
						2,
						3, // Front face.
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
			{
				spine::Skin* s = mSkins[i];
				skin->addSkin(mSkins[i]);
			}
		}
		mSkeleton->setSkin(skin);
		mSkeleton->setSlotsToSetupPose();

		delete prev;
	}

	void FTSpineAnimation::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		FTMeshGroup::SaveProperties(ofs);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTMeshGroup::FRONT_DIR, GetFrontDir());
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTSpineAnimation::SKIN_COMBINATION, static_cast<UINT>(mSkinCombination));
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpineAnimation::JSON_KEY, *mJSONPath);
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpineAnimation::ATLAS_KEY, *mAtlasPath);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
	}

	void FTSpineAnimation::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		FileIOHelper::LoadBasicString(ifs, *mAtlasPath);
		FileIOHelper::LoadBasicString(ifs, *mJSONPath);
		UINT skinCombi = 0;
		FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
		mSkinCombination = static_cast<unsigned char>(skinCombi);

		int frontDir = -1;
		FileIOHelper::LoadInt(ifs, frontDir);

		FTMeshGroup::LoadProperties(ifs);

		0 < frontDir ? SetRightIsFront(true) : SetRightIsFront(false);
	}

	const unsigned char FTSpineAnimation::GetSkinCombination() const
	{
		return mSkinCombination;
	}

	void FTSpineAnimation::SetSkinCombination(const unsigned char skinCombi)
	{
		mSkinCombination = skinCombi;
		SetSkin();
	}

#ifdef FOXTROT_EDITOR

	FTSpineAnimation::FTSpineAnimation(
		Common::FTResourceDef&		resDef,
		D3D11Renderer*				renderer,
		const Common::FTDS::String* jsonPath,
		const Common::FTDS::String* atlasPath)
		: FTSpineAnimation(resDef, renderer)
	{
		mJSONPath->Assign(*jsonPath);
		mAtlasPath->Assign(*atlasPath);
		Process(renderer);
	}

	void FTSpineAnimation::UpdateUI()
	{
		static bool val[MAX_SKIN_COUNT];

		int bitCount = sizeof(mSkinCombination) * 8; // Total number of bits in the integer
		for (int i = bitCount - 1; i >= 0; --i)
		{
			unsigned int mask = 1U << i; // Create a mask with a single bit at position 'i'
			if (mSkinCombination & mask)
				val[i] = true;
		}

		for (size_t i = 0; i < mSkins.size(); ++i)
		{
			Editor::UPDATE_BOOL(mSkins[i]->getName().buffer(), val[i]);
			if (val[i])
				mSkinCombination |= (1 << i);
			else
				mSkinCombination &= ~(1 << i);
		}

		if (ImGui::Button("UpdateSkin"))
			SetSkin();

		FTMeshGroup::UpdateUI();
	}

	void FTSpineAnimation::AddRefCount()
	{
		Common::FTDS::String fileN;
		Common::ExtractFileName(mJSONPath, fileN);
		FTJSON* json = AnimationManager::GetInstance()->GetJSONsFuncGetter()()->GetResource(fileN);
		if (json)
			json->AddRefCount();

		fileN.Clear();

		Common::ExtractFileName(mAtlasPath, fileN);
		FTText* atlas = AnimationManager::GetInstance()->GetTextsFuncGetter()()->GetResource(fileN);
		if (atlas)
			atlas->AddRefCount();

		FTMeshGroup::AddRefCount();
	}

	void FTSpineAnimation::SubtractRefCount()
	{
		Common::FTDS::String fileN;
		Common::ExtractFileName(mJSONPath, fileN);
		FTJSON* json = AnimationManager::GetInstance()->GetJSONsFuncGetter()()->GetResource(fileN);
		if (json)
			json->SubtractRefCount();

		fileN.Clear();

		Common::ExtractFileName(mAtlasPath, fileN);
		FTText* atlas = AnimationManager::GetInstance()->GetTextsFuncGetter()()->GetResource(fileN);
		if (atlas)
			atlas->SubtractRefCount();

		FTMeshGroup::SubtractRefCount();
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11