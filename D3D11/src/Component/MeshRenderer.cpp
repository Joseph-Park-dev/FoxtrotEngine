// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MeshRenderer.h"

#include "Actor/Transform.h"
#include "Actor/Actor.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshDataPack.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/FTTexture.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "TemplateFunctions.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "FTCoreEditor.h"
	#include "EditorUtils.h"
	#include "EditorCamera.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;
	void MeshRenderer::Initialize()
	{
		Component::Initialize();
	}

	void MeshRenderer::ProcessInput(FTInputDevice* inputDevice)
	{
	}

	void MeshRenderer::Update(float deltaTime)
	{
	}

	void MeshRenderer::LateUpdate(float deltaTime)
	{
	}

	void MeshRenderer::Render(Core::FoxtrotRenderer* renderer)
	{
		if (mMeshGroup)
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			// mMeshGroup->Render(renderer, transform, Camera::GetInstance(), mTexture, mVS, mPS, mMaterial);
		}
	}

	void MeshRenderer::CloneTo(Actor* actor)
	{
		MeshRenderer* newComp = DBG_NEW MeshRenderer(GetPlugin(), actor, GetUpdateOrder());
		newComp->mRenderer	  = this->mRenderer;
		newComp->mMeshGroup	  = this->mMeshGroup;
		newComp->mTexture	  = this->mTexture;
		newComp->mVS		  = this->mVS;
		newComp->mPS		  = this->mPS;
		newComp->mMaterial	  = this->mMaterial;
	}

	Core::FoxtrotRenderer* MeshRenderer::GetRenderer() const { return mRenderer; }
	FTMeshGroup*		   MeshRenderer::GetMeshGroup() const { return mMeshGroup; }
	FTTexture*			   MeshRenderer::GetTexture() const { return mTexture; }
	FTVertexShader*		   MeshRenderer::GetVS() const { return mVS; }
	FTGeometryShader*	   MeshRenderer::GetGS() const { return mGS; }
	FTPixelShader*		   MeshRenderer::GetPS() const { return mPS; }
	FTMaterial*			   MeshRenderer::GetMaterial() const { return mMaterial; }

	void MeshRenderer::SetRenderer(Core::FoxtrotRenderer* renderer) { mRenderer = renderer; }
	void MeshRenderer::SetMeshGroup(FTMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
	void MeshRenderer::SetTexture(FTTexture* tex) { mTexture = tex; }
	void MeshRenderer::SetVS(FTVertexShader* vs) { mVS = vs; }
	void MeshRenderer::SetGS(FTGeometryShader* gs) { mGS = gs; }
	void MeshRenderer::SetPS(FTPixelShader* ps) { mPS = ps; }
	void MeshRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }

	MeshRenderer::MeshRenderer(Plugin* plugin, Actor* owner, int updateOrder)
		: D3D11Component(plugin, owner, updateOrder)
		, mMeshGroup(nullptr)
		, mRenderer(nullptr)
		, mTexture(nullptr)
		, mVS(nullptr)
		, mGS(nullptr)
		, mPS(nullptr)
		, mMaterial(nullptr)
		, mFrontDir(1)
	{
	}

	MeshRenderer::~MeshRenderer()
	{
		if (mMeshGroup)
			mMeshGroup = nullptr;

		if (mMaterial)
			mMaterial = nullptr;
	}

	void MeshRenderer::SaveProperties(std::ofstream& ofs)
	{
		Component::SaveProperties(ofs);

		if (mMeshGroup)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, D3D11::RES_NAME(FTMeshGroup, mMeshGroup));
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, Core::ChunkKey::NullVal::NULL_OBJECT);

		if (mTexture)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, D3D11::RES_NAME(FTTexture, mTexture));
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, Core::ChunkKey::NullVal::NULL_OBJECT);

		if (mMaterial)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MAT_KEY, D3D11::RES_NAME(FTMaterial, mMaterial));
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MAT_KEY, Core::ChunkKey::NullVal::NULL_OBJECT);
	}

	void MeshRenderer::LoadProperties(std::ifstream& ifs)
	{
		FTDS::String keyCache; // Temporary space to store the loaded keys.

		// Load material.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		mMaterial = D3D11::GET_RES(FTMaterial, keyCache);

		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMeshGroup::SHADER_KEY);

		// Load Texture.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		// mTexture = ResourceManager::GetInstance()->GetLoadedTexture(keyCache);

		// Load MeshGroup.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		mMeshGroup = D3D11::GET_RES(FTMeshGroup, keyCache);

		Component::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void MeshRenderer::EditorRender(FoxtrotRenderer* renderer)
	{
		if (mMeshGroup)
		{
			renderer->SwitchFillMode();

			Transform* transform = GetOwner()->GetTransform();
			// mMeshGroup->Render(renderer, transform, EditorCamera::GetInstance(), mTexture, mVS, mPS, mMaterial);
		}
	}

	void MeshRenderer::EditorUIUpdate()
	{
		Component::EditorUIUpdate();

		CHECK_RENDERER(GetRenderer());

		if (!mMeshGroup)
			return;

		// mMeshGroup->UpdateUI();

		if (mTexture)
			mTexture->UpdateUI();
		// FTEditorUtils::DisplayResSelection(
		//	"Select Texture",
		//	ResourceManager::GetInstance()->GetSprites(),
		//	mTexture);

		if (mMaterial)
			mMaterial->UpdateUI();
		FTEditorUtils::DisplayResSelection(
			"Select Material",
			ResourceManager::GetInstance()->GetMaterials(),
			mMaterial);
	}
#endif // FOXTROT_EDITOR

	extern "C" __declspec(dllexport) Component* Create(Core::Plugin* plugin, Core::Actor* actor, int updateOrder)
	{
		return DBG_NEW MeshRenderer(plugin, actor, updateOrder);
	}
} // namespace D3D11