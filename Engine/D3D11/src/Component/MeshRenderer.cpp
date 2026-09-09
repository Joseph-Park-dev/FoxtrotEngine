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
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void MeshRenderer::Initialize()
	{
		D3D11Component::Initialize();
	}

	/// @brief Updates the up used by subsequent operations.
	void MeshRenderer::Setup()
	{
		D3D11Component::Setup();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param inputDevice Device exposing the current frame's input state.
	void MeshRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void MeshRenderer::Update(float deltaTime)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void MeshRenderer::LateUpdate(float deltaTime)
	{
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void MeshRenderer::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (mMeshGroup)
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			// mMeshGroup->Render(renderer, transform, Camera::GetInstance(), mTexture, mVS, mPS, mMaterial);
		}
	}

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param actor Actor participating in this operation.
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

	/// @brief Returns the renderer used by this mesh renderer.
	/// @return Borrowed access to the renderer.
	Core::FoxtrotRenderer* MeshRenderer::GetRenderer() const { return mRenderer; }
	/// @brief Returns the mesh group used by this mesh renderer.
	/// @return Borrowed access to the mesh group.
	FTMeshGroup*		   MeshRenderer::GetMeshGroup() const { return mMeshGroup; }
	/// @brief Returns the texture used by this mesh renderer.
	/// @return Borrowed access to the texture.
	FTTexture*			   MeshRenderer::GetTexture() const { return mTexture; }
	/// @brief Returns the vs used by this mesh renderer.
	/// @return Borrowed access to the vs.
	FTVertexShader*		   MeshRenderer::GetVS() const { return mVS; }
	/// @brief Returns the gs used by this mesh renderer.
	/// @return Borrowed access to the gs.
	FTGeometryShader*	   MeshRenderer::GetGS() const { return mGS; }
	/// @brief Returns the ps used by this mesh renderer.
	/// @return Borrowed access to the ps.
	FTPixelShader*		   MeshRenderer::GetPS() const { return mPS; }
	/// @brief Returns the material used by this mesh renderer.
	/// @return Borrowed access to the material.
	FTMaterial*			   MeshRenderer::GetMaterial() const { return mMaterial; }

	/// @brief Updates the renderer used by subsequent operations.
	/// @param renderer Replacement renderer.
	void MeshRenderer::SetRenderer(Core::FoxtrotRenderer* renderer) { mRenderer = renderer; }
	/// @brief Updates the mesh group used by subsequent operations.
	/// @param meshGroup Replacement mesh group.
	void MeshRenderer::SetMeshGroup(FTMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
	/// @brief Updates the texture used by subsequent operations.
	/// @param tex Replacement texture.
	void MeshRenderer::SetTexture(FTTexture* tex) { mTexture = tex; }
	/// @brief Updates the vs used by subsequent operations.
	/// @param vs Replacement vs.
	void MeshRenderer::SetVS(FTVertexShader* vs) { mVS = vs; }
	/// @brief Updates the gs used by subsequent operations.
	/// @param gs Replacement gs.
	void MeshRenderer::SetGS(FTGeometryShader* gs) { mGS = gs; }
	/// @brief Updates the ps used by subsequent operations.
	/// @param ps Replacement ps.
	void MeshRenderer::SetPS(FTPixelShader* ps) { mPS = ps; }
	/// @brief Updates the material used by subsequent operations.
	/// @param mat Replacement material.
	void MeshRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }

	/// @brief Initializes mesh and material state for its owning actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :MeshRenderer base or delegates to its constructor.
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

	/// @brief Completes destruction through the object's inheritance hierarchy.
	MeshRenderer::~MeshRenderer()
	{
		if (mMeshGroup)
			mMeshGroup = nullptr;

		if (mMaterial)
			mMaterial = nullptr;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
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

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
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
	#ifdef FOXTROT_EDITOR
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void MeshRenderer::EditorUpdate(float deltaTime)
	{
	}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void MeshRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}
	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
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

	/// @brief Exposes the material used when drawing this object.
	/// @return Borrowed access to the material.
	/// @note Changes through the returned reference affect this object's stored state.
	FTMaterial*& MeshRenderer::Material()
	{
		return mMaterial;
	}
#endif
} // namespace D3D11
namespace D3D11 {
/// @brief Creates a mesh-rendering component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created mesh renderer instance or resource.
D3D11_API D3D11::MeshRenderer* CreateMeshRenderer(Core::IActor* actor)
	{
		return DBG_NEW D3D11::MeshRenderer(actor);
	}
}
