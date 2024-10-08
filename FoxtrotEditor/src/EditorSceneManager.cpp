#include "EditorSceneManager.h"

#include "Managers/SceneManager.h"

void EditorSceneManager::Init()
{
	mEditorScene->Enter();
}

void EditorSceneManager::ProcessInput(KeyInputManager* keyInputManager) {
	mEditorScene->ProcessInput(keyInputManager);
}

void EditorSceneManager::Update(float deltaTime){
	mEditorScene->Update(deltaTime);
}

void EditorSceneManager::Lateupdate(float deltaTime){
	mEditorScene->LateUpdate(deltaTime);
}

void EditorSceneManager::Render(FoxtrotRenderer* renderer){
	mEditorScene->Render(renderer);
}

void EditorSceneManager::ProcessEvent(){
	mEditorScene->ProcessEvent();
}

EditorScene* EditorSceneManager::GetEditorScene()
{
	return mEditorScene;
}

EditorSceneManager::EditorSceneManager()
	: mEditorScene(new EditorScene)
{}

EditorSceneManager::~EditorSceneManager(){
	delete mEditorScene;
}
