#include "SpineAnimator.h"

#include "spine/spine.h"

#include "Managers/AnimationManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTMeshGroup.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Core/FTCore.h"
#include "Actors/Transform.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

void SpineAnimator::Initialize(FTCore* coreInst)
{
	SetRenderer(coreInst->GetGameRenderer());
	MeshRenderer::Initialize(coreInst);
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

void SpineAnimator::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		renderer->SwitchFillMode();

		Transform* transform = GetOwner()->GetTransform();
		static_cast<FTSpineAnimation*>(
			GetMeshGroup())
			->Render(GetRenderer(), transform, Camera::GetInstance(), GetTexture(), GetVS(), GetPS(), GetMaterial());
	}
}

SpineAnimator::SpineAnimator(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mTimeScale(1.f)
	, mUsePhysics(spine::Physics_None)
{
}

void SpineAnimator::CloneTo(Actor* actor)
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
		FTSpineAnimation*	anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
		const FTDS::String& fn	 = anim->GetFileName();
		FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, fn);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTSpineAnimation::SKIN_COMBINATION, anim->GetSkinCombination());
	}
	else
		FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, ChunkKey::NullVal::NULL_OBJECT);
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

	FTSpineAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpineAnim(key);
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
		static_cast<FTSpineAnimation*>(GetMeshGroup())->UpdateUI();
		MeshRenderer::EditorUIUpdate();
	}
}
#endif