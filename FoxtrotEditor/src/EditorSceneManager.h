#pragma once

#include "Core/SingletonMacro.h"

#include "EditorScene.h"

class EditorSceneManager
{
	SINGLETON(EditorSceneManager)

public:
	virtual void Init();
	virtual void ProcessInput(class KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	virtual void Lateupdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);
	virtual void ProcessEvent();

public:
	EditorScene* GetEditorScene();

private:
	EditorScene* mEditorScene;
};
