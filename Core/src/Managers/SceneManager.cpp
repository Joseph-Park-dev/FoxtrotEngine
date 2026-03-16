// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Managers/SceneManager.h"

#include <fstream>
#include <string>
#include <vector>

#include "Scenes/Scene.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "Managers/CollisionManager.h"
#include "Managers/UIManager.h"

SceneManager::SceneManager()
	: mChunkList()
	, mCurrentScene(DBG_NEW Scene)
{}

SceneManager::~SceneManager()
{
	mChunkList.clear();
	delete mCurrentScene;
}

void SceneManager::SwitchScene(size_t index)
{
	CollisionManager::GetInstance()->Reset();
	UIManager::GetInstance()->Reset();
	mCurrentScene->DeleteAll();
	FTDS::String path = FTDS::String(".\\Chunks\\") + mChunkList.at(index);
	ChunkLoader::GetInstance()->LoadChunk(path);
	mCurrentScene->Setup();
}

Scene* SceneManager::GetCurrentScene()
{
	return mCurrentScene;
}

std::vector<FTDS::String>& SceneManager::GetChunkList()
{
	return mChunkList;
}

void SceneManager::SetChunkListPath(FTDS::String&& path)
{
	mChunkListPath.Assign(path);
}

void SceneManager::Initialize()
{
	SwitchScene(0);
}

void SceneManager::ProcessInput(FTInputDevice* inputDevice)
{
	mCurrentScene->ProcessInput(inputDevice);
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