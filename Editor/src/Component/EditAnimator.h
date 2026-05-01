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
#include "Component/EditorComponent.h"
#include "Component/Animator.h"

#include "TemplateFunctions.h"

namespace Editor
{
	class EditAnimator :
		public Editor::EditorComponent<D3D11::Animator>
	{
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorUIUpdateImpl();
		virtual void EditorRenderImpl(D3D11::D3D11Renderer* renderer);

	private:
		void UpdatePlayAnim();
		void UpdatePlayList();
	};

	namespace ChunkKey
	{
		namespace FTSpriteAnimator
		{
			constexpr const char* LOADED_KEYS = "Loaded Keys";

		} // namespace FTSpriteAnimator
	} // namespace ChunkKey
} // namespace Editor