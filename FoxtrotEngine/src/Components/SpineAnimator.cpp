#include "SpineAnimator.h"

#include "spine/spine.h"

#include "Managers/AnimationManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
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
#endif // FOXTROT_EDITOR

void SpineAnimator::Initialize(FTCore* coreInst)
{
	SetRenderer(coreInst->GetGameRenderer());

	if (0 < mLoadedAnim.size())
		SetMeshGroup(mLoadedAnim.at(0));

	for (FTSpineAnimation* anim : mLoadedAnim)
	{
		if (anim)
		{
			anim->SetMaterials(MaterialKeys(), GetRenderer()->GetDevice());
			anim->SetVertexShader(VSKey());
			anim->SetPixelShader(PSKey());
		}
	}

	Component::Initialize(coreInst);
}

void SpineAnimator::Update(float deltaTime)
{
	if (GetMeshGroup())
		static_cast<FTSpineAnimation*>(GetMeshGroup())
			->Update(deltaTime, spine::Physics_None);
}

void SpineAnimator::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		this->UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		static_cast<FTSpineAnimation*>(GetMeshGroup())->Render(renderer);
	}
}

SpineAnimator::SpineAnimator(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mTimeScale(1.f)
	, mUsePhysics(spine::Physics_Update)
{
}

SpineAnimator::~SpineAnimator()
{
	mLoadedAnim.clear();
}

void SpineAnimator::CloneTo(Actor* actor)
{
	SpineAnimator* newComp = DBG_NEW SpineAnimator(actor, GetUpdateOrder());

	newComp->mLoadedAnim.assign(mLoadedAnim.begin(), mLoadedAnim.end());
	newComp->MaterialKeys().assign(MaterialKeys().begin(), MaterialKeys().end());

	newComp->VSKey().Assign(VSKey());
	newComp->PSKey().Assign(PSKey());
}

void SpineAnimator::UpdateMesh(Transform* transform, Camera* camInst, FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		FTSpineAnimation* anim = static_cast<FTSpineAnimation*>(GetMeshGroup());
		anim->CalcVCData(transform, camInst);
		anim->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
	}
}

void SpineAnimator::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);

	// Loop through material keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	for (size_t i = 0; i < MaterialKeys().size(); ++i)
		FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), MaterialKeys().at(i));
	FileIOHelper::SaveSize(ofs, ChunkKey::MATERIAL_COUNT, MaterialKeys().size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	// Loop through loaded animation keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Animation::LOADED_KEYS);

	size_t i = 0;
	for (FTSpineAnimation* anim : mLoadedAnim)
	{
		FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), mLoadedAnim.at(i)->FileName());
		++i;
	}

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::Animation::LOADED_KEYS);

	// Save Shader keys.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SHADER_KEYS);

	FileIOHelper::SaveString(ofs, ChunkKey::FT_VERTEX_SHADER, VSKey());
	FileIOHelper::SaveString(ofs, ChunkKey::FT_PIXEL_SHADER, PSKey());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SHADER_KEYS);
}

void SpineAnimator::LoadProperties(std::ifstream& ifs)
{
	// Load Shader keys.
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SHADER_KEYS);

	FTDS::String shaderKey;
	FileIOHelper::LoadBasicString(ifs, shaderKey);
	PSKey().Assign(shaderKey);

	shaderKey.Clear();
	FileIOHelper::LoadBasicString(ifs, shaderKey);
	VSKey().Assign(shaderKey);

	// Load Animations
	std::pair<size_t, FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Animation::LOADED_KEYS);
	mLoadedAnim.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		FTDS::String key;
		FileIOHelper::LoadBasicString(ifs, key);

#ifdef FOXTROT_EDITOR
		FTSpineAnimation* anim = EditorResourceManager::GetInstance()->GetLoadedSpineAnim(key);
#else
		FTSpineAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpineAnim(key);
#endif // FOXTROT_EDITOR

		mLoadedAnim.push_back(anim);
		if (mLoadedAnim.size() == 1)
		{
			SetMeshGroup(mLoadedAnim.at(0));
			anim->SetAnimation(0, true);
		}
	}
	std::reverse(mLoadedAnim.begin(), mLoadedAnim.end());

	// Load Materials.
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::MATERIAL_KEYS);
	size_t matCount = 0;
	FileIOHelper::LoadSize(ifs, matCount);
	for (size_t i = 0; i < matCount; ++i)
	{
		FTDS::String key = ChunkKey::NullVal::NULL_OBJECT;
		FileIOHelper::LoadBasicString(ifs, key);
		MaterialKeys().push_back(key);
	}

	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void SpineAnimator::EditorUpdate(float deltaTime)
{
	this->Update(deltaTime);
}

void SpineAnimator::EditorRender(FoxtrotRenderer* renderer)
{
	this->Render(renderer);
}

void SpineAnimator::EditorUIUpdate()
{
	ImGui::Text("Play List");
	FTDS::String key = ChunkKey::NullVal::NULL_OBJECT;
	FTEditorUtils::DisplayResSelection<FTSpineAnimation>(
		"Load Animation",
		EditorResourceManager::GetInstance()->GetSpineAnimations(),
		key);

	if (key.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		FTSpineAnimation* anim = EditorResourceManager::GetInstance()->GetLoadedSpineAnim(key);
		mLoadedAnim.push_back(anim);
		if (mLoadedAnim.size() == 1)
		{
			SetMeshGroup(mLoadedAnim.at(0));
			anim->SetAnimation(0, true);
		}
	}

	if (0 < mLoadedAnim.size())
	{
		size_t i = 0;
		for (FTSpineAnimation* anim : mLoadedAnim)
		{
			ImGui::PushID(anim->FileName().C_Str());
			ImGui::Text(anim->FileName().C_Str());
			anim->UpdateUI();

			if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
				std::iter_swap(mLoadedAnim.begin() + i - 1, mLoadedAnim.begin() + i);
			ImGui::SameLine();
			if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
				std::iter_swap(mLoadedAnim.begin() + i + 1, mLoadedAnim.begin() + i);

			if (ImGui::Button("Delete"))
			{
				mLoadedAnim.erase(mLoadedAnim.begin() + i);
			}
			ImGui::PopID();
			++i;
		}
	}

	if (GetMeshGroup())
	{
		ImGui::SeparatorText("Material");
		UpdateMaterial();

		ImGui::SeparatorText("Shaders");
		UpdateVS();
		UpdatePS();
	}
}
#endif