// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/MeshRenderer.h"

#include "Actor/Transform.h"
#include "Actor/IActor.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshDataPack.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/FTTexture.h"
#include "Renderer/Camera.h"
#include "Renderer/IRenderer.h"
#include "TemplateFunctions.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include "EditorUtils.h"
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	void MeshRenderer::Initialize()
	{
		D3D11Component::Initialize();
	}

	void MeshRenderer::Setup()
	{
		D3D11Component::Setup();
	}

	void MeshRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	void MeshRenderer::Update(float deltaTime)
	{
	}

	void MeshRenderer::LateUpdate(float deltaTime)
	{
	}

	void MeshRenderer::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (mMeshGroup)
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			// mMeshGroup->Render(renderer, transform, Camera::GetInstance(), mTexture, mVS, mPS, mMaterial);
		}
	}

	void MeshRenderer::CloneTo(Core::IActor* actor)
	{
		MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
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

	MeshRenderer::MeshRenderer(Core::IActor* owner, int updateOrder)
		: D3D11Component(owner, updateOrder)
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
		D3D11::D3D11Component::SaveProperties(ofs);

		if (mMeshGroup)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, mMeshGroup->GetFileName());
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, Common::ChunkKey::NullVal::NULL_OBJECT);

		if (mTexture)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, mTexture->GetFileName());
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, Common::ChunkKey::NullVal::NULL_OBJECT);

		if (mMaterial)
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MAT_KEY, mMaterial->GetFileName());
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MAT_KEY, Common::ChunkKey::NullVal::NULL_OBJECT);
	}

	void MeshRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String keyCache; // Temporary space to store the loaded keys.

		// Load material.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		mMaterial = D3D11::ResourceManager::GetInstance()->GetMaterial(keyCache);

		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMeshGroup::SHADER_KEY);

		// Load Texture.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		// mTexture = ResourceManager::GetInstance()->GetLoadedTexture(keyCache);

		// Load MeshGroup.
		FileIOHelper::LoadBasicString(ifs, keyCache);
		mMeshGroup = D3D11::ResourceManager::GetInstance()->GetMeshGroup(keyCache);

		D3D11::D3D11Component::LoadProperties(ifs);
	}
	void MeshRenderer::EditorUpdate(float deltaTime)
	{
	}
	void MeshRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}
	void MeshRenderer::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		D3D11::D3D11Component::EditorUIUpdate(chInst);

		if (!GetMeshGroup())
			return;

		// mMeshGroup->UpdateUI();

		if (GetTexture())
			GetTexture()->UpdateUI();
		// FTEditorUtils::DisplayResSelection(
		//	"Select Texture",
		//	ResourceManager::GetInstance()->GetSprites(),
		//	mTexture);

		if (GetMaterial())
			GetMaterial()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			D3D11::ResourceManager::GetInstance()->GetMaterials(),
			Material());
	}

	FTMaterial*& MeshRenderer::Material()
	{
		return mMaterial;
	}
} // namespace D3D11