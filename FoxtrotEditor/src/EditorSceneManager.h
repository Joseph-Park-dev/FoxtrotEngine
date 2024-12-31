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

class EditorSceneManager
{
	SINGLETON(EditorSceneManager)

public:
	virtual void Initialize();
	virtual void ProcessInput(class KeyInputManager* keyInputManager);

	// Runs on editor when pressing Play button.
	void Update(float deltaTime);

	// Runs on editor when pressing Play button.
	void Render(FoxtrotRenderer* renderer);

	// Additional editor features especially for debugging purpose.
	void EditorUpdate(float deltaTime);
	
	// Additional editor features especially for debugging purpose.
	void EditorRender(FoxtrotRenderer* renderer);

	void ProcessEvent();

	// Deletes all objects in a Scene.
	void DeleteAll();

public:
	EditorScene* GetEditorScene();

private:
	EditorScene* mEditorScene;
};
