// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton object that manages EditorScene & its elements.
/// Note that the class is not inherited from class SceneManager.
/// </summary>

#pragma once
#include "Utility/SingletonMacro.h"
#include <Manager/SceneManager.h>

#include <vector>

#ifdef FOXTROT_EDITOR
	#define FIND_ACTOR_FILTER(name, filter) static_cast<Actor*>(EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(name, filter))
	#define FIND_ACTOR(id) static_cast<Actor*>(EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(id))
#endif

namespace D3D11
{
	class D3D11InputDevice;
}

namespace Editor
{
	class EditorElement;
	class EditorScene;

	class EditorSceneManager :
		public Core::SceneManager
	{
		SINGLETON(EditorSceneManager)
	public:
		// Get the EditorElements with the lowest hierarchyLevel.
		/// @brief Returns the lowests used by this editor scene manager.
		/// @param elements Collection of editor or data elements.
		void				 GetLowests(std::vector<EditorElement*>& elements);
		/// @brief Returns the editor scene used by this editor scene manager.
		/// @return Borrowed access to the editor scene.
		virtual EditorScene* GetEditorScene();

		// EditorElements with lower hierarchyLevel comes first.
		/// @brief Orders editor elements by their hierarchy depth.
		/// @param elements Collection of editor or data elements.
		void SortByHierarchyLv(std::vector<EditorElement*>& elements);

	private:
		/// @brief Appends a row of child actor entries to the editor hierarchy.
		/// @param actor Actor participating in this operation.
		/// @param dest Destination receiving the converted or copied data.
		void PushRowOfChildActors(EditorElement* actor, std::vector<EditorElement*>& dest);
	};
} // namespace Editor
