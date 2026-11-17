// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define IMGUI_DEFINE_MATH_OPERATORS
#include "EditorGizmo.h"

#include <imgui_internal.h>

#include "ImGuizmo/src/ImGuizmo.h"

#include "Actor/Transform.h"
#include "EditorCamera.h"
#include "EditorElement.h"
#include "FTMath.h"

namespace Editor
{
	namespace
	{
		void CopyMatrix(const Math::FTMatrix4& source, float* destination)
		{
			const float* components = source.GetAsFloatPtr();
			for (int index = 0; index < 16; ++index)
				destination[index] = components[index];
		}

		Math::FTMatrix4 MatrixFromComponents(const float* components)
		{
			Math::FTMatrix4 matrix;
			for (int index = 0; index < 16; ++index)
				(&matrix.mat[0][0])[index] = components[index];
			return matrix;
		}

		Math::FTVector3 VectorFromComponents(const float* components)
		{
			return Math::FTVector3(components[0], components[1], components[2]);
		}

		void HoldWindow(ImGuiWindow* window)
		{
			if (!window)
				return;

			window->Flags |= ImGuiWindowFlags_NoMove;
			window->BgClickFlags &= ~ImGuiWindowBgClickFlags_Move;
		}
	} // namespace

	/// @brief Initializes translate mode in world space.
	EditorGizmo::EditorGizmo()
		: mOperation(ImGuizmo::TRANSLATE)
		, mMode(ImGuizmo::WORLD)
		, mMatrix{}
		, mActor(nullptr)
		, mUsing(false)
		, mToolbarHovered(false)
	{
		for (int index = 0; index < 16; ++index)
			mMatrix[index] = 0.f;
		mMatrix[0]	= 1.f;
		mMatrix[5]	= 1.f;
		mMatrix[10] = 1.f;
		mMatrix[15] = 1.f;
	}

	/// @brief Prepares ImGuizmo for the current ImGui frame.
	void EditorGizmo::BeginFrame()
	{
		ImGuizmo::BeginFrame();
	}

	/// @brief Drops a window move ImGui scheduled while the gizmo was dragging.
	void EditorGizmo::BlockWindowMove()
	{
		if (!mUsing || !ImGui::GetCurrentContext())
			return;

		ImGui::GetCurrentContext()->MovingWindow = nullptr;
	}

	/// @brief Draws the move, rotate, and scale controls over the viewport.
	/// @param viewportPosition Screen-space minimum of the scene image.
	void EditorGizmo::DrawToolbar(const ImVec2& viewportPosition)
	{
		ImGui::SetCursorScreenPos(ImVec2(viewportPosition.x + 8.f, viewportPosition.y + 8.f));
		ImGui::BeginGroup();

		if (ImGui::RadioButton("Move", mOperation == ImGuizmo::TRANSLATE))
			mOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mOperation == ImGuizmo::ROTATE))
			mOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mOperation == ImGuizmo::SCALE))
			mOperation = ImGuizmo::SCALE;

		if (mOperation != ImGuizmo::SCALE)
		{
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", mMode == ImGuizmo::LOCAL))
				mMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mMode == ImGuizmo::WORLD))
				mMode = ImGuizmo::WORLD;
		}

		ImGui::EndGroup();
		mToolbarHovered = ImGui::IsItemHovered();
		if (mToolbarHovered)
			ImGui::SetTooltip("T move, E rotate, R scale. Hold Ctrl to snap.");
	}

	/// @brief Switches the active operation from the viewport hotkeys.
	/// @param viewportHovered True when the cursor is over the scene image.
	void EditorGizmo::HandleHotkeys(bool viewportHovered)
	{
		if (!viewportHovered || mToolbarHovered || mUsing || ImGui::GetIO().WantTextInput)
			return;

		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mOperation = ImGuizmo::TRANSLATE;
		else if (ImGui::IsKeyPressed(ImGuiKey_E))
			mOperation = ImGuizmo::ROTATE;
		else if (ImGui::IsKeyPressed(ImGuiKey_R))
			mOperation = ImGuizmo::SCALE;
	}

	/// @brief Decomposes the edited matrix into the actor's local and world transform.
	/// @param actor Actor receiving the decomposed transform.
	void EditorGizmo::ApplyToActor(EditorElement* actor)
	{
		Core::Transform* transform = actor->GetTransform();
		if (!transform)
			return;

		const Math::FTMatrix4 world = MatrixFromComponents(mMatrix);
		Math::FTMatrix4		  local = world;

		const Common::IActor* owner	 = transform->GetOwner();
		const Common::IActor* parent = owner ? owner->GetParent() : nullptr;
		if (parent && parent->GetTransform())
		{
			// World is local multiplied by the parent, so local is world times the parent inverse.
			Math::FTMatrix4 parentInverse = parent->GetTransform()->GetMatrixWorld();
			parentInverse.Invert();
			local = world * parentInverse;
		}

		float translation[3];
		float rotation[3];
		float scale[3];
		ImGuizmo::DecomposeMatrixToComponents(local.GetAsFloatPtr(), translation, rotation, scale);
		transform->SetLocalPosition(VectorFromComponents(translation));
		transform->SetLocalScale(VectorFromComponents(scale));
		transform->SetLocalRotation(Core::Transform::ConvertDegreeToRad(VectorFromComponents(rotation)));

		ImGuizmo::DecomposeMatrixToComponents(world.GetAsFloatPtr(), translation, rotation, scale);
		transform->SetWorldPosition(VectorFromComponents(translation));
		transform->SetWorldScale(VectorFromComponents(scale));
		transform->SetWorldRotation(Core::Transform::ConvertDegreeToRad(VectorFromComponents(rotation)));
	}

	/// @brief Draws the viewport gizmo and writes changes into the actor transform.
	/// @param actor Actor being edited. May be null when nothing is selected.
	/// @param camera Editor camera that produced the viewport image.
	/// @param viewportPosition Screen-space minimum of the scene image.
	/// @param viewportSize Size of the scene image in pixels.
	/// @param viewportHovered True when the cursor is over the scene image.
	void EditorGizmo::Manipulate(
		EditorElement* actor,
		EditorCamera*  camera,
		const ImVec2&  viewportPosition,
		const ImVec2&  viewportSize,
		bool		   viewportHovered)
	{
		if (viewportSize.x < 1.f || viewportSize.y < 1.f)
		{
			mUsing = false;
			mActor = nullptr;
			return;
		}

		DrawToolbar(viewportPosition);
		HandleHotkeys(viewportHovered);

		Core::Transform* transform = actor ? actor->GetTransform() : nullptr;
		if (!actor || !transform || !camera)
		{
			mUsing = false;
			mActor = nullptr;
			ImGuizmo::Enable(true);
			return;
		}

		if (!mUsing || mActor != actor)
			CopyMatrix(transform->GetMatrixWorld(), mMatrix);
		mActor = actor;

		Math::FTMatrix4 view;
		Math::FTMatrix4 projection;
		camera->GetViewMatrix(view);
		camera->GetProjectionMatrix(projection);

		ImGuizmo::Enable(!mToolbarHovered || mUsing);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(viewportPosition.x, viewportPosition.y, viewportSize.x, viewportSize.y);
		ImGuizmo::SetOrthographic(camera->GetViewType() == Core::Viewtype::Orthographic);

		float snap[3] = { 0.25f, 0.25f, 0.25f };
		if (mOperation == ImGuizmo::ROTATE)
		{
			snap[0] = 15.f;
			snap[1] = 15.f;
			snap[2] = 15.f;
		}
		else if (mOperation == ImGuizmo::SCALE)
		{
			snap[0] = 0.1f;
			snap[1] = 0.1f;
			snap[2] = 0.1f;
		}

		const bool useSnap = ImGui::GetIO().KeyCtrl;
		ImGuizmo::PushID(actor->GetID());
		const bool edited = ImGuizmo::Manipulate(
			view.GetAsFloatPtr(),
			projection.GetAsFloatPtr(),
			static_cast<ImGuizmo::OPERATION>(mOperation),
			static_cast<ImGuizmo::MODE>(mMode),
			mMatrix,
			nullptr,
			useSnap ? snap : nullptr);
		ImGuizmo::PopID();

		// IsUsing() is only true while this gizmo's ID is pushed. IsUsingAny() stays true for the drag.
		mUsing = ImGuizmo::IsUsingAny();
		if (edited)
			ApplyToActor(actor);
		if (mUsing)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			HoldWindow(window);
			HoldWindow(window->RootWindow);
			HoldWindow(window->RootWindowDockTree);
			if (window->DockNode)
				HoldWindow(window->DockNode->HostWindow);
		}

		ImGuizmo::Enable(true);
	}
} // namespace Editor
