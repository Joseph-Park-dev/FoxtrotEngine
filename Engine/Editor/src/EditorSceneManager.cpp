// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorSceneManager.h"

#include <algorithm>

#include "InputSystem/D3D11InputDevice.h"
#include "Manager/SceneManager.h"
#include "Actor/Actor.h"
#include "EditorScene.h"
#include "EditorElement.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Editor
{
	/// @brief Returns the lowests used by this editor scene manager.
	/// @param elements Collection of editor or data elements.
	void EditorSceneManager::GetLowests(std::vector<EditorElement*>& elements)
	{
		Common::FTDS::DynamicArray<Core::IActor*>*& actors = Core::SceneManager::GetCurrentScene()->Actors();

		for (auto iter = actors->Begin(); iter != actors->End(); ++iter)
		{
			EditorElement* ele = static_cast<EditorElement*>(*iter);
			ele->SetIsDisplayed(false);
			if (ele->GetHierarchyLevel() < 1)
				elements.push_back(ele);
		}
	}

	/// @brief Returns the editor scene used by this editor scene manager.
	/// @return Borrowed access to the editor scene.
	EditorScene* EditorSceneManager::GetEditorScene()
	{
		return reinterpret_cast<EditorScene*>(GetCurrentScene());
	}

	/// @brief Orders editor elements by their hierarchy depth.
	/// @param elements Collection of editor or data elements.
	void EditorSceneManager::SortByHierarchyLv(std::vector<EditorElement*>& elements)
	{
		std::sort(elements.begin(), elements.end(), [](const EditorElement* lhs, const EditorElement* rhs) {
			return lhs->GetHierarchyLevel() < rhs->GetHierarchyLevel();
		});
	}

	/// @brief Appends a row of child actor entries to the editor hierarchy.
	/// @param actor Actor participating in this operation.
	/// @param dest Destination receiving the converted or copied data.
	void EditorSceneManager::PushRowOfChildActors(EditorElement* actor, std::vector<EditorElement*>& dest)
	{
		dest.push_back(actor);
		Common::FTDS::DynamicArray<Core::IActor*>* childActors = actor->GetChildActors();
		for (auto child = childActors->Begin(); child != childActors->End(); ++child)
		{
			EditorElement* elemChild = static_cast<EditorElement*>(*child);
			PushRowOfChildActors(elemChild, dest);
		}
	}

	/// @brief Initializes editor scene selection and configuration state.
	/// @note Initializes the :EditorSceneManager base or delegates to its constructor.
	EditorSceneManager::EditorSceneManager()
		: Core::SceneManager()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	EditorSceneManager::~EditorSceneManager()
	{
	}
} // namespace Editor
