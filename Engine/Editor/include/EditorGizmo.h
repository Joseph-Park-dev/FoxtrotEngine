// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Viewport gizmo that edits the focused actor's transform.
/// </summary>

#pragma once

#include <imgui.h>

namespace Editor
{
	class EditorCamera;
	class EditorElement;

	class EditorGizmo
	{
	public:
		/// @brief Initializes translate mode in world space.
		EditorGizmo();

		/// @brief Prepares ImGuizmo for the current ImGui frame.
		/// @note Call once, immediately after ImGui::NewFrame().
		void BeginFrame();

		/// @brief Drops a window move ImGui scheduled while the gizmo was dragging.
		/// @note Call before ImGui::NewFrame and again after ImGui::EndFrame.
		void BlockWindowMove();

		/// @brief Draws the viewport gizmo and writes changes into the actor transform.
		/// @param actor Actor being edited. May be null when nothing is selected.
		/// @param camera Editor camera that produced the viewport image.
		/// @param viewportPosition Screen-space minimum of the scene image.
		/// @param viewportSize Size of the scene image in pixels.
		/// @param viewportHovered True when the cursor is over the scene image.
		void Manipulate(
			EditorElement* actor,
			EditorCamera*  camera,
			const ImVec2&  viewportPosition,
			const ImVec2&  viewportSize,
			bool		   viewportHovered);

	private:
		/// @brief Draws the move, rotate, and scale controls over the viewport.
		/// @param viewportPosition Screen-space minimum of the scene image.
		void DrawToolbar(const ImVec2& viewportPosition);

		/// @brief Switches the active operation from the viewport hotkeys.
		/// @param viewportHovered True when the cursor is over the scene image.
		void HandleHotkeys(bool viewportHovered);

		/// @brief Decomposes the edited matrix into the actor's local and world transform.
		/// @param actor Actor receiving the decomposed transform.
		void ApplyToActor(EditorElement* actor);

	private:
		int			   mOperation;
		int			   mMode;
		float		   mMatrix[16];
		EditorElement* mActor;
		bool		   mUsing;
		bool		   mToolbarHovered;
	};
} // namespace Editor
