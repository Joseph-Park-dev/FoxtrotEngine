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
#include "Renderer/FoxtrotRenderer.h"

#include "EditorSceneManager.h"
#include "EditorLayer.h"
#include "EditorElement.h"
#include "EditorChunkLoader.h"
#include "ActorCommand.h"

namespace Editor
{
	void EditorScene::DeleteAll()
	{
		UnfocusEditorElements();
		Scene::DeleteAll();
	}

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

	EditorElement* EditorScene::AddEditorElement()
	{
		UnfocusEditorElements();
		EditorChunkLoader::GetInstance()->AddMaxActorID();
		int			   maxID		 = EditorChunkLoader::GetInstance()->GetMaxActorID();
		EditorElement* editorElement = DBG_NEW EditorElement(maxID);

		Common::FTDS::String& name = editorElement->GetNameRef();
		name.Append(std::to_string(Actors()->GetSize()).c_str());

		editorElement->SetIsFocused(true);

		AddActor(editorElement);
		return editorElement;
	}

	EditorElement* EditorScene::AddEditorElement(Core::Actor* actor)
	{
		UnfocusEditorElements();

		EditorElement* element = DBG_NEW EditorElement(actor, actor->GetID(), false);
		AddActor(element);
		return element;
	}

	EditorElement* EditorScene::AddEditorElement(Core::Actor* actor, int id)
	{
		UnfocusEditorElements();

		EditorElement* element = DBG_NEW EditorElement(actor, id, false);
		AddActor(element);
		return element;
	}

	void EditorScene::EditorUpdate(float deltaTime)
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
	}

	EditorScene::EditorScene()
	{
	}

	EditorScene::~EditorScene()
	{
		DeleteAll();
	}

	EditorScene* GetEditorScene()
	{
		return EditorSceneManager::GetInstance()->GetEditorScene();
	}
} // namespace Editor