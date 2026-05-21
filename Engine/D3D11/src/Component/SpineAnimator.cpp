#include "Component/SpineAnimator.h"

#include <spine/spine.h>

#include "Manager/AnimationManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/FTSpineLoader.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Mesh/Vertex.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/FTMeshDataPack.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"
#include "Actor/IActor.h"
#include "Actor/Transform.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;

	void SpineAnimator::Play(const int idx, bool isRepeated)
	{
		static_cast<FTSpineAnimation*>(GetMeshGroup())->SetAnimation(idx, isRepeated);
	}

	void SpineAnimator::Initialize()
	{
		MeshRenderer::Initialize();
	}

	void SpineAnimator::ProcessInput(IInputDevice* inputDevice)
	{
	}

	void SpineAnimator::Update(float deltaTime)
	{
		if (GetMeshGroup())
		{
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Update(deltaTime, mUsePhysics);
		}
	}

	void SpineAnimator::LateUpdate(float deltaTime)
	{
	}

	void SpineAnimator::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Render(renderer, transform, Camera::GetInstance(), GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	SpineAnimator::SpineAnimator(Core::IActor* owner, int updateOrder)
		: MeshRenderer(owner, updateOrder)
		, mTimeScale(1.f)
		, mUsePhysics(spine::Physics_None)
	{
	}

	void SpineAnimator::CloneTo(Core::IActor* actor)
	{
		SpineAnimator* newComp = DBG_NEW SpineAnimator(actor, GetUpdateOrder());
		newComp->SetRenderer(this->GetRenderer());
		newComp->SetMeshGroup(this->GetMeshGroup());
		newComp->SetTexture(this->GetTexture());
		newComp->SetVS(this->GetVS());
		newComp->SetPS(this->GetPS());
		newComp->SetMaterial(this->GetMaterial());
	}

	void SpineAnimator::SaveProperties(std::ofstream& ofs)
	{
		MeshRenderer::SaveProperties(ofs);
		if (GetMeshGroup())
		{
			FTSpineAnimation* anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
			FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, anim->GetFileName());
			FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTSpineAnimation::SKIN_COMBINATION, anim->GetSkinCombination());
		}
		else
			FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, Common::ChunkKey::NullVal::NULL_OBJECT);
	}

	void SpineAnimator::LoadProperties(std::ifstream& ifs)
	{
		UINT skinCombi = 0;
		FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
		unsigned char skinCombination = static_cast<unsigned char>(skinCombi);

		// Load Animation
		Common::FTDS::String key;
		FileIOHelper::LoadBasicString(ifs, key);
		MeshRenderer::LoadProperties(ifs);

		FTSpineAnimation* anim = D3D11::ResourceManager::GetInstance()->GetSpineAnimation(key);
		if (anim)
		{
			SetMeshGroup(anim);
			anim->SetAnimation(0, true);
			anim->SetSkinCombination(skinCombination);
		}
	}
} // namespace D3D11