// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorSceneManager.h"

#include "Managers/SceneManager.h"

void EditorSceneManager::Initialize()
{}

void EditorSceneManager::ProcessInput(KeyInputManager* keyInputManager) {
	mEditorScene->ProcessInput(keyInputManager);
}

void EditorSceneManager::Update(float deltaTime){
	mEditorScene->Update(deltaTime);
	mEditorScene->LateUpdate(deltaTime);
}

void EditorSceneManager::Render(FoxtrotRenderer* renderer){
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

void EditorSceneManager::ProcessEvent(){
	mEditorScene->ProcessEvent();
}

void EditorSceneManager::DeleteAll()
{
	if(mEditorScene)
		mEditorScene->DeleteAll();
	delete mEditorScene;
	mEditorScene = nullptr;
}

EditorScene* EditorSceneManager::GetEditorScene()
{
	return mEditorScene;
}

EditorSceneManager::EditorSceneManager()
	: mEditorScene(DBG_NEW EditorScene)
{}

EditorSceneManager::~EditorSceneManager(){
	delete mEditorScene;
	mEditorScene = nullptr;
}
