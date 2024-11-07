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
	virtual void Update(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

	// Runs on editor BEFORE pressing Play button.
	// Additional editor features especially for debugging purpose.
	virtual void EditorUpdate(float deltaTime);
	virtual void EditorRender(FoxtrotRenderer* renderer);

	virtual void ProcessEvent();

public:
	EditorScene* GetEditorScene();

private:
	EditorScene* mEditorScene;
};
