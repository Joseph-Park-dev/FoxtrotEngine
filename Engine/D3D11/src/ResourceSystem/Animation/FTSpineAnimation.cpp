#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"

#include <spine/spine.h>
#include <wrl.h>
#include <d3d11.h>

#include "Manager/AnimationManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/FTSpineLoader.h"
#include "Manager/DirectoryHelper.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/FTTexture.h"
#include "Actor/Transform.h"
#include "Actor/Steering.h"
#include "FTMath.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"
#include "Utility/D3D11Utils.h"
#include "ResourceSystem/D3D11PSO.h"

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
	using namespace Math;
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

	void FTSpineAnimation::UpdateConstantBuffers(Core::IRenderer* renderer, Core::Transform* transform, Core::ICamera* camInst, FTMaterial* mat, const size_t, const int)
	{
		if (!renderer || !transform || !camInst || !mSpineVCData || !mSpineVCBuf || !mSpinePSData || !mSpinePSBuf)
			return;

		// Facing flips the skeleton on the model matrix. The actor scale stays positive.
		float linearX = transform->GetSteering()->Linear.x;
		if (linearX < 0)
			SetDirection(-1);
		else if (0 < linearX)
			SetDirection(1);

		FTVector3 scale = transform->GetLocalScale();
		if (scale.x < 0.f || scale.y < 0.f)
			transform->SetLocalScale(FTVector3(Math::Abs(scale.x), Math::Abs(scale.y), scale.z));

		const float facing	 = static_cast<float>(GetFrontDir() * GetDirection());
		FTMatrix4	modelMat = FTMatrix4::CreateScale(facing, 1.f, 1.f) * transform->GetMatrixWorld();

		FTMatrix4 viewMat = FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);

		FTMatrix4 projMat = FTMatrix4::Identity;
		camInst->GetProjectionMatrix(projMat);

		// TextureVSSpine reads model, view, and projection from register b0.
		mSpineVCData->Model = modelMat.Transposed();
		mSpineVCData->View	= viewMat.Transposed();
		mSpineVCData->Proj	= projMat.Transposed();
		D3D11Utils::UpdateBuffer(
			static_cast<D3D11Renderer*>(renderer)->GetContext(), *mSpineVCData, mSpineVCBuf);

		D3D11Utils::UpdateBuffer(
			static_cast<D3D11Renderer*>(renderer)->GetContext(), *mSpinePSData, mSpinePSBuf);

		if (mat)
			mat->UpdateBuffer(static_cast<D3D11Renderer*>(renderer)->GetContext());
	}

	void FTSpineAnimation::Render(Core::IRenderer* renderer, Core::Transform* transform, Core::ICamera* camInst, D3D11PSO* pso, FTMaterial* mat)
	{
		if (!renderer || !pso || !pso->IsValid() || !mMeshes || !mSkeleton)
			return;

		UpdateConstantBuffers(renderer, transform, camInst, mat);

		ComPtr<ID3D11Device>&		 device	 = static_cast<D3D11Renderer*>(renderer)->GetDevice();
		ComPtr<ID3D11DeviceContext>& context = static_cast<D3D11Renderer*>(renderer)->GetContext();
		UpdateBuffers(device, context);
		pso->SetToPipeline(context);

		context->VSSetConstantBuffers(0, 1, mSpineVCBuf.GetAddressOf());
		if (mSpinePSBuf)
			context->PSSetConstantBuffers(0, 1, mSpinePSBuf.GetAddressOf());

		// Index 0 is the back-most slot. Later slots are drawn on top.
		auto& drawOrder = mSkeleton->getDrawOrder();
		for (size_t i = 0; i < drawOrder.size(); ++i)
		{
			spine::Slot* slot = drawOrder[i];
			if (!slot)
				continue;

			const int slotIndex = slot->getData().getIndex();
			if (slotIndex < 0 || mMeshes->GetSize() <= static_cast<size_t>(slotIndex))
				continue;

			SpineMesh* mesh = mMeshes->At(static_cast<size_t>(slotIndex));
			if (!mesh || !mesh->Active)
				continue;

			FTTexture* pageTex = mesh->Texture ? mesh->Texture : GetTexture();
			if (pageTex)
			{
				ID3D11ShaderResourceView* srv = pageTex->GetSRV().Get();
				context->PSSetShaderResources(0, 1, &srv);
			}

			mesh->Draw(context);
		}
	}

	void FTSpineAnimation::SetTimeScale(const float val)
	{
		mTimeScale = val;
	}

	void FTSpineAnimation::SetAnimation(const int idx, const bool isRepeated)
	{
		if (!mState || idx < 0 || mLoadedClips.size() <= static_cast<size_t>(idx))
			return;
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

	FTSpineAnimation::FTSpineAnimation(Common::FTResourceDef& resDef, void* renderer)
		: FTSprite(resDef, renderer, true)
		, mJSONPath(DBG_NEW Common::FTDS::String)
		, mAtlasPath(DBG_NEW Common::FTDS::String)
		, mTimeScale(1.f)
		, mInitRightward(true)
		, mSkinCombination(0x0)
		, mCurrAnimIdx(-1)
		, mAtlas(nullptr)
		, mSkeletonData(nullptr)
		, mSkeleton(nullptr)
		, mStateData(nullptr)
		, mState(nullptr)
		, mMeshes(DBG_NEW Common::FTDS::DynamicArray<SpineMesh*>)
		, mSpineVCData(nullptr)
		, mSpinePSData(nullptr)
	{
		Process(renderer);
		ApplyInitialDirection();
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
		delete mSpineVCData;
		delete mSpinePSData;
		mSpineVCData = nullptr;
		mSpinePSData = nullptr;
		delete mJSONPath;
		delete mAtlasPath;
		mJSONPath  = nullptr;
		mAtlasPath = nullptr;
	}

	void FTSpineAnimation::Process(void* renderer)
	{
		if (!mMeshes->IsEmpty())
			return;

		// A .spineanim on disk supplies the JSON and atlas paths.
		// Paths already assigned by the editor constructor are left in place.
		if (mJSONPath->IsEmpty() || mAtlasPath->IsEmpty())
		{
			std::ifstream ifs(GetRelativePath()->C_Str());
			if (ifs.good())
				this->LoadProperties(ifs);
		}

		if (mJSONPath->IsEmpty() || mAtlasPath->IsEmpty())
			return;

		InitializeSpinAnim(static_cast<D3D11Renderer*>(renderer)->GetDevice());
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

		Core::DirectoryHelper::GetInstance()->RelativeToAbsolutePath(*mAtlasPath);
		Core::DirectoryHelper::GetInstance()->RelativeToAbsolutePath(*mJSONPath);

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

		auto& slots = mSkeleton->getSlots();
		mMeshes->Reserve(slots.size() > 0 ? slots.size() : 1);
		for (size_t i = 0; i < slots.size(); ++i)
			mMeshes->PushBack(nullptr);

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

	void FTSpineAnimation::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
	{
		FTSprite::InitializeConstantBuffers(device);

		if (!mSpineVCData)
			mSpineVCData = DBG_NEW SpineVCData;

		mSpineVCBuf.Reset();
		D3D11Utils::CreateConstantBuffer(device, *mSpineVCData, mSpineVCBuf);

		if (!mSpinePSData)
			mSpinePSData = DBG_NEW SpinePSData;
		mSpinePSBuf.Reset();
		D3D11Utils::CreateConstantBuffer(device, *mSpinePSData, mSpinePSBuf);
	}

	SpineMesh* FTSpineAnimation::EnsureSlotMesh(
		ComPtr<ID3D11Device>&		 device,
		int							 slotIndex,
		D3D11::SPINE_ATTACHMENT_TYPE attachmentType,
		size_t						 vertexCount,
		size_t						 indexCount)
	{
		if (slotIndex < 0 || vertexCount == 0)
			return nullptr;

		while (mMeshes->GetSize() <= static_cast<size_t>(slotIndex))
			mMeshes->PushBack(nullptr);

		SpineMesh* mesh		= mMeshes->At(static_cast<size_t>(slotIndex));
		const bool reusable = mesh && mesh->VertexCount >= vertexCount && mesh->IndexCount >= indexCount && mesh->PositionBuf && mesh->TexcoordBuf && (indexCount == 0 || mesh->IndexBuf);
		if (!reusable)
		{
			delete mesh;
			mesh = DBG_NEW SpineMesh;
			if (FAILED(mesh->ResourceBinding(device, slotIndex, attachmentType, vertexCount, indexCount)))
			{
				delete mesh;
				mesh = nullptr;
			}
			mMeshes->At(static_cast<size_t>(slotIndex)) = mesh;
		}

		if (!mesh)
			return nullptr;

		mesh->MeshType		  = attachmentType;
		mesh->DrawOrder		  = slotIndex;
		mesh->DrawVertexCount = static_cast<UINT>(vertexCount);
		mesh->DrawIndexCount  = static_cast<UINT>(indexCount);
		return mesh;
	}

	static FTTexture* AtlasTexture(spine::TextureRegion* region)
	{
		if (!region)
			return nullptr;

		auto* atlasRegion = static_cast<spine::AtlasRegion*>(region);
		if (!atlasRegion->page)
			return nullptr;

		return static_cast<FTTexture*>(atlasRegion->page->texture);
	}

	void FTSpineAnimation::UpdateBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	{
		if (!mSkeleton || !mMeshes)
			return;

		auto& slots = mSkeleton->getSlots();
		while (mMeshes->GetSize() < slots.size())
			mMeshes->PushBack(nullptr);

		for (size_t i = 0; i < slots.size(); ++i)
		{
			spine::Slot* slot = slots[i];
			if (!slot)
				continue;

			const int  slotIndex = slot->getData().getIndex();
			SpineMesh* mesh		 = nullptr;
			if (0 <= slotIndex && static_cast<size_t>(slotIndex) < mMeshes->GetSize())
			{
				mesh = mMeshes->At(static_cast<size_t>(slotIndex));
				if (mesh)
				{
					mesh->Active  = false;
					mesh->Texture = nullptr;
				}
			}

			spine::Attachment* attachment = slot->getAttachment();
			if (!attachment)
				continue;

			// For spine mesh attachment
			if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
			{
				spine::MeshAttachment* attm		   = static_cast<spine::MeshAttachment*>(attachment);
				const size_t		   vertexCount = attm->getWorldVerticesLength() / 2;
				const size_t		   indexCount  = attm->getTriangles().size();
				mesh							   = EnsureSlotMesh(device, slotIndex, D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_ATTACH, vertexCount, indexCount);
				if (!mesh)
					continue;

				mesh->Texture = AtlasTexture(attm->getRegion());

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
					if (mesh->IndexBuf && SUCCEEDED(context->Map(mesh->IndexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
					{
						memcpy(mapped.pData, indices, sizeof(uint16_t) * indexCount);
						context->Unmap(mesh->IndexBuf.Get(), 0);
					}
				}
				mesh->Active = true;
			}
			// For spine mesh region, which is a square.
			else if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
			{
				auto* region = static_cast<spine::RegionAttachment*>(attachment);
				mesh		 = EnsureSlotMesh(device, slotIndex, D3D11::SPINE_ATTACHMENT_TYPE::SPINE_MESH_REGION, 4, 6);
				if (!mesh)
					continue;

				mesh->Texture = AtlasTexture(region->getRegion());

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
					if (mesh->IndexBuf && SUCCEEDED(context->Map(mesh->IndexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
					{
						memcpy(mapped.pData, indices, sizeof(uint16_t) * indexCount);
						context->Unmap(mesh->IndexBuf.Get(), 0);
					}
				}
				mesh->Active = true;
			}
		}
	}

	void FTSpineAnimation::SetSkin()
	{
		spine::Skin* prev = mSkeleton->getSkin();
		spine::Skin* skin = new spine::Skin("Skin");

		const size_t skinCount = mSkins.size() < MAX_SKIN_COUNT ? mSkins.size() : MAX_SKIN_COUNT;
		for (size_t i = 0; i < skinCount; ++i)
		{
			const unsigned int mask = 1u << static_cast<unsigned int>(i);
			if (mSkinCombination & mask)
				skin->addSkin(mSkins[i]);
		}
		mSkeleton->setSkin(skin);
		mSkeleton->setSlotsToSetupPose();

		delete prev;
	}

	void FTSpineAnimation::ApplyInitialDirection()
	{
		SetDirection(mInitRightward ? 1 : -1);
	}

	void FTSpineAnimation::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		FTMeshGroup::SaveProperties(ofs);
		FileIOHelper::SaveBool(ofs, ChunkKey::FTSpineAnimation::INIT_RIGHTWARD, mInitRightward);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTMeshGroup::FRONT_DIR, GetFrontDir());
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTSpineAnimation::SKIN_COMBINATION, static_cast<UINT>(mSkinCombination));
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpineAnimation::JSON_KEY, *mJSONPath);
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpineAnimation::ATLAS_KEY, *mAtlasPath);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
	}

	void FTSpineAnimation::LoadProperties(std::ifstream& ifs)
	{
		std::pair<size_t, Common::FTDS::String> pack =
			FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpineAnimation::FT_SPINE_ANIMATION);
		FileIOHelper::LoadBasicString(ifs, *mAtlasPath);
		FileIOHelper::LoadBasicString(ifs, *mJSONPath);
		UINT skinCombi = 0;
		FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
		mSkinCombination = skinCombi;

		int frontDir = -1;
		FileIOHelper::LoadInt(ifs, frontDir);

		FTMeshGroup::LoadProperties(ifs);

		0 < frontDir ? SetRightIsFront(true) : SetRightIsFront(false);

		FileIOHelper::LoadBool(ifs, mInitRightward);
		ApplyInitialDirection();
	}

	const unsigned int FTSpineAnimation::GetSkinCombination() const
	{
		return mSkinCombination;
	}

	void FTSpineAnimation::SetSkinCombination(const unsigned int skinCombi)
	{
		mSkinCombination = skinCombi;
		SetSkin();
	}

#ifdef FOXTROT_EDITOR

	FTSpineAnimation::FTSpineAnimation(
		Common::FTResourceDef&		resDef,
		Core::IRenderer*			renderer,
		const Common::FTDS::String* jsonPath,
		const Common::FTDS::String* atlasPath)
		: FTSprite(resDef, renderer, true)
		, mJSONPath(DBG_NEW Common::FTDS::String)
		, mAtlasPath(DBG_NEW Common::FTDS::String)
		, mTimeScale(1.f)
		, mInitRightward(true)
		, mSkinCombination(0x0)
		, mCurrAnimIdx(-1)
		, mAtlas(nullptr)
		, mSkeletonData(nullptr)
		, mSkeleton(nullptr)
		, mStateData(nullptr)
		, mState(nullptr)
		, mMeshes(DBG_NEW Common::FTDS::DynamicArray<SpineMesh*>)
		, mSpineVCData(nullptr)
		, mSpinePSData(nullptr)
	{
		// The .spineanim path is not an image. Build from the supplied JSON and atlas.
		if (jsonPath)
			mJSONPath->Assign(*jsonPath);
		if (atlasPath)
			mAtlasPath->Assign(*atlasPath);
		Process(renderer);
		ApplyInitialDirection();
	}

	void FTSpineAnimation::UpdateUI()
	{
		ImGui::Checkbox("Initial Rightward", &mInitRightward);
		ApplyInitialDirection();

		ImGui::SeparatorText("Skins");

		if (mSkins.size() == 0)
			ImGui::TextUnformatted("This skeleton has no skins.");

		bool		 skinChanged = false;
		const size_t skinCount	 = mSkins.size() < MAX_SKIN_COUNT ? mSkins.size() : MAX_SKIN_COUNT;
		for (size_t i = 0; i < skinCount; ++i)
		{
			const unsigned int bit	   = 1u << static_cast<unsigned int>(i);
			bool			   enabled = (mSkinCombination & bit) != 0;

			const char* name = mSkins[i] ? mSkins[i]->getName().buffer() : nullptr;
			if (!name || name[0] == '\0')
				name = "Unnamed Skin";

			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Checkbox(name, &enabled))
				skinChanged = true;
			ImGui::PopID();

			if (enabled)
				mSkinCombination |= bit;
			else
				mSkinCombination &= ~bit;
		}
		if (skinChanged || ImGui::Button("UpdateSkin"))
			SetSkin();

		FTMeshGroup::UpdateUI();
	}

	void FTSpineAnimation::AddRefCount()
	{
		// Common::FTDS::String fileN;
		// Common::ExtractFileName(mJSONPath, fileN);
		// GenericData::FTJSON* json = AnimationManager::GetInstance()->GetJSONsFuncGetter()()->GetResource(fileN);
		// if (json)
		//	json->AddRefCount();

		// fileN.Clear();

		// Common::ExtractFileName(mAtlasPath, fileN);
		// GenericData::FTText* atlas = AnimationManager::GetInstance()->GetTextsFuncGetter()()->GetResource(fileN);
		// if (atlas)
		//	atlas->AddRefCount();

		FTMeshGroup::AddRefCount();
	}

	void FTSpineAnimation::SubtractRefCount()
	{
		// Common::FTDS::String fileN;
		// Common::ExtractFileName(mJSONPath, fileN);
		// GenericData::FTJSON* json = AnimationManager::GetInstance()->GetJSONsFuncGetter()()->GetResource(fileN);
		// if (json)
		//	json->SubtractRefCount();

		// fileN.Clear();

		// Common::ExtractFileName(mAtlasPath, fileN);
		// GenericData::FTText* atlas = AnimationManager::GetInstance()->GetTextsFuncGetter()()->GetResource(fileN);
		// if (atlas)
		//	atlas->SubtractRefCount();

		FTMeshGroup::SubtractRefCount();
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11