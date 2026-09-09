// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// An object that emulates the game scene.
/// This shows how the Actors are arranged & interacts in a Scene
/// in the produced game.
/// </summary>

#pragma once
#include "Scene/Scene.h"

#include <vector>

namespace Editor
{
	class EditorElement;

	class EditorScene :
		public Core::Scene
	{
	public:
		// Sets all EditorElements' focused status to false.
		/// @brief Clears selection and focus from the editor elements.
		void UnfocusEditorElements();

		// Adds an empty EditorElement.
		/// @brief Registers an editor element for hierarchy, selection, and lifecycle processing.
		/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
		Editor::EditorElement* AddEditorElement();

		/// @brief Registers an editor element for hierarchy, selection, and lifecycle processing.
		/// Adds an EditorElement with copied values from actor.
		/// Useful when Stopping a scene on FTEditor.
		/// <param name="actor : ">Actor to copy values from.</param>
		/// @param actor Actor participating in this operation.
		/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
		virtual Editor::EditorElement* AddEditorElement(Common::IActor* actor);

		/// @brief Adds an EditorElement with new id.
		/// Useful when making Premade instance.
		/// @param id This should have new id.
		/// @param actor Actor participating in this operation.
		/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
		virtual Editor::EditorElement* AddEditorElement(Common::IActor* actor, int id);

	public:
		// Deletes all EditorElements & clearing the Scene.
		/// @brief Deletes the scene's managed actors and clears its collections.
		void DeleteAll() override;

	public:
		// Updates editor specific features -> this will be omitted from the produced game.
		// void EditorUpdate(float deltaTime);

		// Renders editor specific features -> this will be omitted from the produced game.
		// void EditorRender(Core::FoxtrotRenderer* renderer);

	public:
		/// @brief Initializes scene state used by editor hierarchy operations.
		EditorScene();
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~EditorScene();
	};

#include "Plugin/EditorExports.h"

	constexpr const char*			   GET_EDITOR_SCENE_FUNC = "GetEditorScene";
	/// @brief Returns the editor scene used by this service.
	/// @return Borrowed access to the editor scene.
	extern "C" EDITOR_API EditorScene* GetEditorScene();
} // namespace Editor
