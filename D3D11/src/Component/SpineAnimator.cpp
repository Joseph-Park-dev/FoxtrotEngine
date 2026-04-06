#include "SpineAnimator.h"

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
#include "FTCore.h"
#include "Actor/Actor.h"
#include "Actor/Transform.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;

	void SpineAnimator::Play(const int idx, bool isRepeated)
	{
		static_cast<FTSpineAnimation*>(GetMeshGroup())->SetAnimation(idx, isRepeated);
	}

	void SpineAnimator::Initialize()
	{
		MeshRenderer::Initialize();
	}

	void SpineAnimator::ProcessInput(FTInputDevice* inputDevice)
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

	void SpineAnimator::Render(Core::FoxtrotRenderer* renderer)
	{
		D3D11Renderer* rend = static_cast<D3D11Renderer*>(renderer);
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Render(rend, transform, Camera::GetInstance(), GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	SpineAnimator::SpineAnimator(Plugin* plugin, Actor* owner, int updateOrder)
		: MeshRenderer(plugin, owner, updateOrder)
		, mTimeScale(1.f)
		, mUsePhysics(spine::Physics_None)
	{
	}

	void SpineAnimator::CloneTo(Actor* actor)
	{
		SpineAnimator* newComp = DBG_NEW SpineAnimator(GetPlugin(), actor, GetUpdateOrder());
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
			FTSpineAnimation*	anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
			const FTDS::String& fn	 = D3D11::RES_NAME(FTSpineAnimation, anim);
			FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, fn);
			FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTSpineAnimation::SKIN_COMBINATION, anim->GetSkinCombination());
		}
		else
			FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, Core::ChunkKey::NullVal::NULL_OBJECT);
	}

	void SpineAnimator::LoadProperties(std::ifstream& ifs)
	{
		UINT skinCombi = 0;
		FileIOHelper::LoadUnsignedInt(ifs, skinCombi);
		unsigned char skinCombination = static_cast<unsigned char>(skinCombi);

		// Load Animation
		FTDS::String key;
		FileIOHelper::LoadBasicString(ifs, key);
		MeshRenderer::LoadProperties(ifs);

		FTSpineAnimation* anim = D3D11::GET_RES(FTSpineAnimation, key);
		if (anim)
		{
			SetMeshGroup(anim);
			anim->SetAnimation(0, true);
			anim->SetSkinCombination(skinCombination);
		}
	}

#ifdef FOXTROT_EDITOR
	void SpineAnimator::EditorUpdate(float deltaTime)
	{
		this->Update(deltaTime);
	}

	void SpineAnimator::EditorRender(FoxtrotRenderer* renderer)
	{
		if (GetMeshGroup())
		{
			renderer->SwitchFillMode();

			Transform* transform = GetOwner()->GetTransform();
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Render(GetRenderer(), transform, EditorCamera::GetInstance(), GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	void SpineAnimator::EditorUIUpdate()
	{
		ImGui::Text("Play List");
		FTSpineAnimation* anim = nullptr;
		FTEditorUtils::DisplayResSelection<FTSpineAnimation>(
			"Load Animation",
			ResourceManager::GetInstance()->GetSpineAnimations(),
			anim);

		if (anim)
		{
			SetMeshGroup(anim);
			anim->SetAnimation(1, true);
		}

		if (GetMeshGroup())
		{
			FTSpineAnimation* anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
			anim->UpdateUI();
			if (ImGui::Button("Update"))
				AnimationManager::GetInstance()->SaveAnimationAsFile(anim);
			MeshRenderer::EditorUIUpdate();
		}
	}
#endif

	extern "C" __declspec(dllexport) Component* Create(Core::Plugin* plugin, Core::Actor* actor, int updateOrder)
	{
		return DBG_NEW SpineAnimator(plugin, actor, updateOrder);
	}
} // namespace D3D11