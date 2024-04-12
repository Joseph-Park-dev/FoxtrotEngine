#include "SceneManager.h"
#include "Scene_Start.h"
#include "Scene_Tool.h"

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
	mArrScene[(Uint32)SCENE_TYPE::START] = new Scene_Start;
	mArrScene[(Uint32)SCENE_TYPE::START]->SetName(L"Start Scene");

	mArrScene[(Uint32)SCENE_TYPE::TOOL] = new Scene_Tool;
	mArrScene[(Uint32)SCENE_TYPE::TOOL]->SetName(L"Tool Scene");

	mPCurrScene = mArrScene[(Uint32)SCENE_TYPE::START];
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

void SceneManager::Render(SDL_Renderer* renderer)
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
	mPCurrScene = mArrScene[(Uint32)sceneType];
	mPCurrScene->Enter();
}

#ifdef _DEBUG
void SceneManager::EditorUpdate(float deltaTime)
{
	mPCurrScene->EditorUpdate(deltaTime);
}

void SceneManager::EditorLateUpdate(float deltaTime)
{
	mPCurrScene->EditorLateUpdate(deltaTime);
}
#endif // _DEBUG