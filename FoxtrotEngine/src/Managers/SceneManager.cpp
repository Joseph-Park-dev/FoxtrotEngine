#include "Managers/SceneManager.h"

#include <fstream>
#include <string>

#include "Scenes/Scene.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/ChunkLoader.h"

SceneManager::SceneManager()
	: mChunkList()
	, mCurrentScene(new Scene)
{}

SceneManager::~SceneManager()
{
	mChunkList.clear();
	delete mCurrentScene;
}

void SceneManager::SwitchScene(size_t index)
{
	mCurrentScene->DeleteAll();
	ChunkLoader::GetInstance()->LoadChunk(std::string(".\\Chunks\\") + mChunkList.at(index));
}

Scene* SceneManager::GetCurrentScene()
{
	return mCurrentScene;
}

std::vector<std::string>& SceneManager::GetChunkList()
{
	return mChunkList;
}

void SceneManager::SetChunkListPath(std::string&& path)
{
	mChunkListPath.assign(path);
}

void SceneManager::Initialize()
{
	SwitchScene(0);
}

void SceneManager::ProcessInput(KeyInputManager* keyInputManager)
{
	mCurrentScene->ProcessInput(keyInputManager);
}

void SceneManager::Update(float deltaTime)
{
	mCurrentScene->Update(deltaTime);
}

void SceneManager::Lateupdate(float deltaTime)
{
	mCurrentScene->LateUpdate(deltaTime);
}

void SceneManager::Render(FoxtrotRenderer* renderer)
{
	mCurrentScene->Render(renderer);
}

void SceneManager::ProcessEvent()
{
	mCurrentScene->ProcessEvent();
}

void SceneManager::SaveSceneList (std::ofstream& ofs)
{
}

//void SceneManager::LoadSceneList()
//{
//	std::ifstream ifs(mChunkListPath);
//	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs);
//	for (size_t i = 0; i < pack.first; ++i)
//	{
//		std::string title = {};
//		FileIOHelper::LoadBasicString(ifs, title);
//		mChunkList.push_back(title);
//	}
//}