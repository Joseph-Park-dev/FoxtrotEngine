#include "Managers/SceneManager.h"

#include "Scenes/Scene_Start.h"
//#include "Scenes/Scene_Tool.h"
#include "Renderer/FoxtrotRenderer.h"

SceneManager::SceneManager()
	: mArrScene{}
	, mPCurrScene(nullptr)
{}

SceneManager::~SceneManager()
{
	for (unsigned int i = 0; i < (unsigned int)SCENE_TYPE::END; ++i)
	{
		if (mArrScene[i] != nullptr)
		{
			delete mArrScene[i];
		}
	}
}

void SceneManager::Init()
{
	mArrScene[(UINT)SCENE_TYPE::START] = new Scene_Start;
	mArrScene[(UINT)SCENE_TYPE::START]->SetName(L"Start Scene");

	mPCurrScene = mArrScene[(UINT)SCENE_TYPE::START];
	mPCurrScene->Enter();
}

void SceneManager::ProcessInput(KeyInputManager* keyInputManager)
{
	mPCurrScene->ProcessInput(keyInputManager);
}

void SceneManager::Update(float deltaTime)
{
	mPCurrScene->Update(deltaTime);
}

void SceneManager::Lateupdate(float deltaTime)
{
	mPCurrScene->LateUpdate(deltaTime);
}

void SceneManager::Render(FoxtrotRenderer* renderer)
{
	mPCurrScene->Render(renderer);
}

void SceneManager::ProcessEvent()
{
	mPCurrScene->ProcessEvent();
}

void SceneManager::SwitchScene(SCENE_TYPE sceneType)
{
	mPCurrScene->Exit();
	mPCurrScene = mArrScene[(UINT)sceneType];
	mPCurrScene->Enter();
}


void SceneManager::EditorProcessInput(KeyInputManager* keyInputManager) {
	mPCurrScene->EditorProcessInput(keyInputManager);
}

void SceneManager::EditorUpdate(float deltaTime)
{
	mPCurrScene->EditorUpdate(deltaTime);
}

void SceneManager::EditorLateUpdate(float deltaTime)
{
	mPCurrScene->EditorLateUpdate(deltaTime);
}

void SceneManager::EditorRender(FoxtrotRenderer* renderer)
{
	mPCurrScene->EditorRender(renderer);
}
 // _DEBUG