#include "Component/EditorSpineAnimator.h"

#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "EditorUtils.h"
#include "EditorCamera.h"

#include <Manager/AnimationManager.h>
#include <../D3D11/include/Manager/ResourceManager.h>

namespace Editor
{
	void EditorSpineAnimator::EditorUpdate(float deltaTime)
	{
		this->Update(deltaTime);
	}

	void EditorSpineAnimator::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			EditorRenderer*	 rend	   = reinterpret_cast<EditorRenderer*>(renderer);
			static_cast<D3D11::FTSpineAnimation*>(
				GetMeshGroup())
				->Render(rend, transform, camInst, GetTexture(), GetVS(), GetPS(), GetMaterial());
		}
	}

	void EditorSpineAnimator::EditorUIUpdate()
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
		}
		if (!GetMeshGroup())
			return;

		D3D11::FTSpineAnimation* anim = static_cast<D3D11::FTSpineAnimation*>(GetMeshGroup());
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
} // namespace Editor