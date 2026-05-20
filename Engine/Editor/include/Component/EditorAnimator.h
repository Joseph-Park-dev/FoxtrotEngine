// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Takes care of all FTAnimations registered.
/// All types of animation are handled here as FTResource derived from
/// the same class, FTAnimation.
/// (sprite animation, spine animation, etc.)
/// </summary>

#pragma once
#include "Component/IEditorComponent.h"
#include "Component/Animator.h"

#include "TemplateFunctions.h"

namespace Editor
{
	class EditAnimator :
		public IEditorComponent,
		public D3D11::Animator
	{
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate() override;

	private:
		void UpdatePlayAnim();
		void UpdatePlayList();
	};
} // namespace Editor