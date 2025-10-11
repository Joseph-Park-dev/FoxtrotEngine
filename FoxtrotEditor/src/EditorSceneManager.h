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
#include "Core/SingletonMacro.h"

#include "EditorScene.h"

#ifdef FOXTROT_EDITOR
	#define FIND_ACTOR(name, filter) static_cast<Actor*>(EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(name, filter))
	#define FIND_ACTOR(id) static_cast<Actor*>(EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(id))
#endif

class EditorSceneManager
{
	SINGLETON(EditorSceneManager)

public:
	virtual void Initialize();
	virtual void ProcessInput(class FTInputDevice* inputDevice);

	// Runs on editor when pressing Play button.
	void Update(float deltaTime);

	// Runs on editor when pressing Play button.
	void Render(FoxtrotRenderer* renderer);

	// Additional editor features especially for debugging purpose.
	void EditorUpdate(float deltaTime);

	// Additional editor features especially for debugging purpose.
	void EditorRender(FoxtrotRenderer* renderer);

	// Deletes all objects in a Scene.
	void DeleteAll();

	// Get the EditorElements with the lowest hierarchyLevel.
	void GetLowests(std::vector<EditorElement*>& elements);

	// EditorElements with lower hierarchyLevel comes first.
	void SortByHierarchyLv(std::vector<EditorElement*>& elements);

public:
	EditorScene* GetEditorScene();

private:
	EditorScene* mEditorScene;

private:
	void PushRowOfChildActors(EditorElement* actor, std::vector<EditorElement*>& dest);
};
