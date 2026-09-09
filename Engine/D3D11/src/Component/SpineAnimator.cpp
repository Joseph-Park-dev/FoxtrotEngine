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

	/// @brief Starts or selects animation playback.
	/// @param idx Zero-based element index.
	/// @param isRepeated Whether playback repeats after its final frame.
	void SpineAnimator::Play(const int idx, bool isRepeated)
	{
		static_cast<FTSpineAnimation*>(GetMeshGroup())->SetAnimation(idx, isRepeated);
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void SpineAnimator::Initialize()
	{
		MeshRenderer::Initialize();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param inputDevice Device exposing the current frame's input state.
	void SpineAnimator::ProcessInput(IInputDevice* inputDevice)
	{
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpineAnimator::Update(float deltaTime)
	{
		if (GetMeshGroup())
		{
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Update(deltaTime, mUsePhysics);
		}
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpineAnimator::LateUpdate(float deltaTime)
	{
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void SpineAnimator::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			static_cast<FTSpineAnimation*>(
				GetMeshGroup())
				->Render(renderer, transform, camInst, GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	/// @brief Initializes Spine animation playback state for its actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :SpineAnimator base or delegates to its constructor.
	SpineAnimator::SpineAnimator(Core::IActor* owner, int updateOrder)
		: MeshRenderer(owner, updateOrder)
		, mTimeScale(1.f)
		, mUsePhysics(spine::Physics_None)
	{
	}

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param actor Actor participating in this operation.
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

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
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

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
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

	#ifdef FOXTROT_EDITOR
	/// @brief Advances state needed only while operating in the editor.
	/// @param deltaTime Elapsed frame time in seconds.
	void SpineAnimator::EditorUpdate(float deltaTime)
	{
		Update(deltaTime);
	}

	/// @brief Submits editor-specific overlays and viewport graphics.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void SpineAnimator::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			static_cast<D3D11::FTSpineAnimation*>(
				GetMeshGroup())
				->Render(renderer, transform, camInst, GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
	void SpineAnimator::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		ImGui::Text("Play List");
		D3D11::FTSpineAnimation* anim = nullptr;
		Editor::DisplayResSelection<D3D11::FTSpineAnimation>(
			"Load Animation",
			D3D11::ResourceManager::GetInstance()->GetSpineAnimations(),
			anim);

		if (anim)
		{
			SetMeshGroup(anim);
			anim->SetAnimation(1, true);

			anim = static_cast<D3D11::FTSpineAnimation*>(GetMeshGroup());
			anim->UpdateUI();
			if (ImGui::Button("Update"))
				D3D11::AnimationManager::GetInstance()->SaveAnimationAsFile(anim);

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
	}
#endif
} // namespace D3D11
namespace D3D11 {
/// @brief Creates a Spine animation component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created spine animator instance or resource.
D3D11_API D3D11::SpineAnimator* CreateSpineAnimator(Core::IActor* actor)
	{
		return DBG_NEW D3D11::SpineAnimator(actor);
	}
}
