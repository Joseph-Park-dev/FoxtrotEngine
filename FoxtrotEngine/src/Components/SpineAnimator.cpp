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
			GetMeshGroup())->Update(deltaTime, mUsePhysics);
	}
}

void SpineAnimator::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		this->UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		static_cast<FTSpineAnimation*>(GetMeshGroup())->Render(GetRenderer(), GetTexture(), GetVS(), GetPS(), GetMaterial());
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

void SpineAnimator::UpdateMesh(Transform* transform, Camera* camInst, FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		FTSpineAnimation* anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
		anim->CalcVCData(transform, camInst);
		anim->UpdateConstantBuffers(
			renderer->GetDevice(),
			renderer->GetContext(),
			GetMaterial());
	}
}

void SpineAnimator::SaveProperties(std::ofstream& ofs)
{
	MeshRenderer::SaveProperties(ofs);
	FTDS::String& fn = static_cast<FTSpineAnimation*>(GetMeshGroup())->FileName();
	FileIOHelper::SaveString(ofs, ChunkKey::SpineAnimator::LOADED_ANIM, fn);
}

void SpineAnimator::LoadProperties(std::ifstream& ifs)
{
	// Load Animation
	FTDS::String key;
	FileIOHelper::LoadBasicString(ifs, key);
	MeshRenderer::LoadProperties(ifs);

	FTSpineAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpineAnim(key);
	SetMeshGroup(anim);
	anim->SetAnimation(0, true);
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
		this->UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		static_cast<FTSpineAnimation*>(GetMeshGroup())->Render(renderer, GetTexture(), GetVS(), GetPS(), GetMaterial());
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
		anim->SetAnimation(0, true);
	}

	MeshRenderer::EditorUIUpdate();
}
#endif