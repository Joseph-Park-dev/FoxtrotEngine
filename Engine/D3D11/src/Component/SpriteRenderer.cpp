// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/SpriteRenderer.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "FTMath.h"
#include "Renderer/Camera.h"
#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/D3D11PSO.h"
#include "Scene/Scene.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorUtils.h"
	#include <EditorCamera.h>

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <ImGuiFileDialog/ImGuiFileDialog.h>
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void SpriteRenderer::Initialize()
	{
		D3D11Component::Initialize();
	}

	/// @brief Updates the up used by subsequent operations.
	void SpriteRenderer::Setup()
	{
		D3D11Component::Setup();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param inputDevice Device exposing the current frame's input state.
	void SpriteRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpriteRenderer::Update(float deltaTime)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpriteRenderer::LateUpdate(float deltaTime)
	{
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void SpriteRenderer::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), camInst, mMaterial);
			mSprite->Render(renderer, GetOwner()->GetTransform(), camInst, mPSO, mMaterial);
		}
	}

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param actor Actor participating in this operation.
	void SpriteRenderer::CloneTo(Core::IActor* actor)
	{
		SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
		newComp->mSprite		= this->mSprite;
		newComp->mPSO			= this->mPSO;
		newComp->mMaterial		= this->mMaterial;
	}

	/// @brief Initializes sprite and material bindings for its actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :SpriteRenderer base or delegates to its constructor.
	SpriteRenderer::SpriteRenderer(IActor* owner, int updateOrder)
		: D3D11Component(owner, updateOrder)
		, mSprite(nullptr)
		, mMaterial(nullptr)
	{
		mPSO = D3D11::ResourceManager::GetInstance()->GetPSO(ChunkKey::SpriteRenderer::PSO);
	}

	/// @brief Returns the sprite used by this sprite renderer.
	/// @return Borrowed access to the sprite.
	FTSprite* SpriteRenderer::GetSprite() const
	{
		return mSprite;
	}

	/// @brief Returns the material used by this sprite renderer.
	/// @return Borrowed access to the material.
	FTMaterial* SpriteRenderer::GetMaterial() const
	{
		return mMaterial;
	}

	/// @brief Returns the pso used by this sprite renderer.
	/// @return Borrowed access to the pso.
	D3D11PSO* SpriteRenderer::GetPSO() const
	{
		return mPSO;
	}

	/// @brief Updates the sprite used by subsequent operations.
	/// @param sprite Replacement sprite.
	void SpriteRenderer::SetSprite(FTSprite* sprite) { mSprite = sprite; }
	/// @brief Updates the material used by subsequent operations.
	/// @param mat Replacement material.
	void SpriteRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }
	/// @brief Updates the pso used by subsequent operations.
	/// @param pso Replacement pso.
	void SpriteRenderer::SetPSO(D3D11PSO* pso) { mPSO = pso; }

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void SpriteRenderer::SaveProperties(std::ofstream& ofs)
	{
		D3D11::D3D11Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::SPRITE, mSprite->GetFileName());
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::MATERIAL, mMaterial->GetFileName());
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void SpriteRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String matKey;
		FileIOHelper::LoadBasicString(ifs, matKey);

		Common::FTDS::String spriteKey;
		FileIOHelper::LoadBasicString(ifs, spriteKey);
		D3D11::D3D11Component::LoadProperties(ifs);

		mSprite	  = D3D11::ResourceManager::GetInstance()->GetSprite(spriteKey);
		mMaterial = D3D11::ResourceManager::GetInstance()->GetMaterial(matKey);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpriteRenderer::EditorUpdate(float deltaTime)
	{
	}

	/// @brief Submits editor-specific overlays and viewport graphics.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void SpriteRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetSprite())
		{
			GetSprite()->UpdateConstantBuffers(
				renderer,
				GetOwner()->GetTransform(),
				camInst,
				Material());

			GetSprite()->Render(
				renderer,
				GetOwner()->GetTransform(),
				camInst,
				PSO(),
				Material());
		}
	}

	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
	void SpriteRenderer::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		D3D11::D3D11Component::EditorUIUpdate(chInst);

		if (GetSprite())
			GetSprite()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Sprite",
			D3D11::ResourceManager::GetInstance()->GetSprites(),
			Sprite());

		if (GetMaterial())
			GetMaterial()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			D3D11::ResourceManager::GetInstance()->GetMaterials(),
			Material());
	}

	/// @brief Exposes the sprite resource selected for this renderer.
	/// @return Borrowed access to the sprite.
	/// @note Changes through the returned reference affect this object's stored state.
	FTSprite*& SpriteRenderer::Sprite()
	{
		return mSprite;
	}
	/// @brief Exposes the material used when drawing this object.
	/// @return Borrowed access to the material.
	/// @note Changes through the returned reference affect this object's stored state.
	FTMaterial*& SpriteRenderer::Material()
	{
		return mMaterial;
	}
	/// @brief Exposes the pipeline state used for drawing this object.
	/// @return Borrowed access to the pso.
	/// @note Changes through the returned reference affect this object's stored state.
	D3D11PSO*& SpriteRenderer::PSO()
	{
		return mPSO;
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
namespace D3D11 {
/// @brief Creates a sprite-rendering component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created sprite renderer instance or resource.
D3D11_API D3D11::SpriteRenderer* CreateSpriteRenderer(Core::IActor* actor)
	{
		return DBG_NEW D3D11::SpriteRenderer(actor);
	}
}
