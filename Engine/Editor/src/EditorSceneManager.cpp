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

	EditorScene* EditorSceneManager::GetEditorScene()
	{
		return reinterpret_cast<EditorScene*>(GetCurrentScene());
	}

	void EditorSceneManager::SortByHierarchyLv(std::vector<EditorElement*>& elements)
	{
		std::sort(elements.begin(), elements.end(), [](const EditorElement* lhs, const EditorElement* rhs) {
			return lhs->GetHierarchyLevel() < rhs->GetHierarchyLevel();
		});
	}

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

	EditorSceneManager::EditorSceneManager()
		: Core::SceneManager()
	{
	}

	EditorSceneManager::~EditorSceneManager()
	{
	}
} // namespace Editor