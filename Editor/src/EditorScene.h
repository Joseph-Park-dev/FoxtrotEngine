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

class EditorElement;

namespace Editor
{
	class EditorScene :
		public Core::Scene
	{
	public:
		// Sets all EditorElements' focused status to false.
		void UnfocusEditorElements();

		// Adds an empty EditorElement.
		EditorElement* AddEditorElement();

		/// <summary>
		/// Adds an EditorElement with copied values from actor.
		/// Useful when Stopping a scene on FTEditor.
		/// </summary>
		/// <param name="actor : ">Actor to copy values from.</param>
		virtual EditorElement* AddEditorElement(Core::Actor* actor);

		/// @brief Adds an EditorElement with new id.
		/// Useful when making Premade instance.
		/// @param id This should have new id.
		virtual EditorElement* AddEditorElement(Core::Actor* actor, int id);

	public:
		// Deletes all EditorElements & clearing the Scene.
		void DeleteAll() override;

	public:
		// Updates editor specific features -> this will be omitted from the produced game.
		void EditorUpdate(float deltaTime);

		// Renders editor specific features -> this will be omitted from the produced game.
		void EditorRender(Core::FoxtrotRenderer* renderer);

	public:
		EditorScene();
		~EditorScene();
	};
} // namespace Editor