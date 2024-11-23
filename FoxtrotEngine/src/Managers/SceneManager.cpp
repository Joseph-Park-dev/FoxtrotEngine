#include "Managers/SceneManager.h"

#include "Scenes/Scene.h"
#include "Renderer/FoxtrotRenderer.h"

SceneManager::SceneManager()
	: mArrScene{}
	, mPCurrScene(new Scene)
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
	delete mPCurrScene;
}

void SceneManager::Initialize()
{
	//mArrScene[(UINT)SCENE_TYPE::START] = new Scene_Start;
	//mArrScene[(UINT)SCENE_TYPE::START]->SetName(L"Start Scene");
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

void SceneManager::DeleteAll()
{
	for (size_t i = 0; i < (size_t)SCENE_TYPE::END; ++i)
	{
		if (mArrScene[i])
		{
			mArrScene[i]->DeleteAll();
			delete mArrScene[i];
		}
	}
}

void SceneManager::SwitchScene(SCENE_TYPE sceneType)
{
	mPCurrScene->Exit();
	mPCurrScene = mArrScene[(UINT)sceneType];
	mPCurrScene->Enter();
}