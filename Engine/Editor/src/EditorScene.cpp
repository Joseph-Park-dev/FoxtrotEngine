// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorScene.h"

#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"

#include "EditorLayer.h"
#include "EditorElement.h"
#include "ChunkLoader.h"
#include "ActorCommand.h"

namespace Editor
{
	/// @brief Deletes the scene's managed actors and clears its collections.
	void EditorScene::DeleteAll()
	{
		UnfocusEditorElements();
		Scene::DeleteAll();
	}

	/// @brief Clears selection and focus from the editor elements.
	void EditorScene::UnfocusEditorElements()
	{
		if (!EditorLayer::GetInstance()->FocusedEditorElement())
			return;

		for (auto iter = Actors()->Begin(); iter != Actors()->End(); ++iter)
		{
			EditorElement* ele = static_cast<EditorElement*>((*iter));
			if (ele->GetIsFocused())
				ele->SetIsFocused(false);
		}
	}

	/// @brief Registers an editor element for hierarchy, selection, and lifecycle processing.
	/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
	EditorElement* EditorScene::AddEditorElement()
	{
		UnfocusEditorElements();
		Editor::ChunkLoader::GetInstance()->AddMaxActorID();
		int			   maxID		 = Editor::ChunkLoader::GetInstance()->GetMaxActorID();
		EditorElement* editorElement = DBG_NEW EditorElement(maxID);

		Common::FTDS::String& name = editorElement->GetNameRef();
		name.Append(std::to_string(Actors()->GetSize()).c_str());

		editorElement->SetIsFocused(true);

		AddActor(editorElement);
		return editorElement;
	}

	/// @brief Registers an editor element for hierarchy, selection, and lifecycle processing.
	/// @param actor Actor participating in this operation.
	/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
	EditorElement* EditorScene::AddEditorElement(Core::IActor* actor)
	{
		UnfocusEditorElements();

		EditorElement* element = DBG_NEW EditorElement(actor, actor->GetID(), false);
		AddActor(element);
		return element;
	}

	/// @brief Registers an editor element for hierarchy, selection, and lifecycle processing.
	/// @param actor Actor participating in this operation.
	/// @param id Actor or entry identifier.
	/// @return Registers an editor element for hierarchy, selection, and lifecycle processing.
	EditorElement* EditorScene::AddEditorElement(Core::IActor* actor, int id)
	{
		UnfocusEditorElements();

		EditorElement* element = DBG_NEW EditorElement(actor, id, false);
		AddActor(element);
		return element;
	}

	/*void EditorScene::EditorUpdate(float deltaTime)
	{
		SetIsUpdatingActors(true);

		for (auto iter = Actors()->Begin(); iter != Actors()->End(); ++iter)
		{
			EditorElement* ele = static_cast<EditorElement*>((*iter));
			ele->EditorUpdate(deltaTime);
		}

		SetIsUpdatingActors(false);
	}

	void EditorScene::EditorRender(Core::FoxtrotRenderer* renderer)
	{
		SetIsUpdatingActors(true);

		for (auto iter = Actors()->Begin(); iter != Actors()->End(); ++iter)
		{
			EditorElement* ele = static_cast<EditorElement*>((*iter));
			ele->EditorRender(renderer);
		}

		SetIsUpdatingActors(false);
	}*/

	/// @brief Initializes scene state used by editor hierarchy operations.
	/// @note Initializes the :EditorScene base or delegates to its constructor.
	EditorScene::EditorScene()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	EditorScene::~EditorScene()
	{
		DeleteAll();
	}
} // namespace Editor
