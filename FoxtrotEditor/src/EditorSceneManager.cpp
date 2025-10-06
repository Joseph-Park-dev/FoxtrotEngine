// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorSceneManager.h"

#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "EditorScene.h"
#include "EditorElement.h"

void EditorSceneManager::Initialize()
{
}

void EditorSceneManager::ProcessInput(FTInputDevice* inputDevice)
{
	mEditorScene->ProcessInput(inputDevice);
}

void EditorSceneManager::Update(float deltaTime)
{
	mEditorScene->Update(deltaTime);
	mEditorScene->LateUpdate(deltaTime);
}

void EditorSceneManager::Render(FoxtrotRenderer* renderer)
{
	mEditorScene->Render(renderer);
}

void EditorSceneManager::EditorUpdate(float deltaTime)
{
	mEditorScene->EditorUpdate(deltaTime);
}

void EditorSceneManager::EditorRender(FoxtrotRenderer* renderer)
{
	mEditorScene->EditorRender(renderer);
}

void EditorSceneManager::DeleteAll()
{
	delete mEditorScene;
	mEditorScene = nullptr;
}

void EditorSceneManager::GetLowests(std::vector<EditorElement*>& elements)
{
	FTDS::DynamicArray<Actor*>* actors = GetEditorScene()->Actors();

	for (auto iter = actors->Begin(); iter != actors->End(); ++iter)
	{
		EditorElement* ele = static_cast<EditorElement*>(*iter);
		ele->SetIsDisplayed(false);
		if (ele->GetHierarchyLevel() < 1)
			elements.push_back(ele);
	}
}

void EditorSceneManager::SortByHierarchyLv(std::vector<EditorElement*>& elements)
{
	std::sort(elements.begin(), elements.end(), [](const EditorElement* lhs, const EditorElement* rhs) {
		return lhs->GetHierarchyLevel() < rhs->GetHierarchyLevel();
	});
}

EditorScene* EditorSceneManager::GetEditorScene()
{
	return mEditorScene;
}

void EditorSceneManager::PushRowOfChildActors(EditorElement* actor, std::vector<EditorElement*>& dest)
{
	dest.push_back(actor);
	FTDS::DynamicArray<Actor*>& childActors = actor->GetChildActors();
	for (auto child = childActors.Begin(); child != childActors.End(); ++child)
	{
		EditorElement* elemChild = static_cast<EditorElement*>(*child);
		PushRowOfChildActors(elemChild, dest);
	}
}

EditorSceneManager::EditorSceneManager()
	: mEditorScene(DBG_NEW EditorScene)
{
}

EditorSceneManager::~EditorSceneManager()
{
	delete mEditorScene;
	mEditorScene = nullptr;
}
