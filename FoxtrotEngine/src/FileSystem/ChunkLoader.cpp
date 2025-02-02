// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/ChunkLoader.h"

#include <string>
#include <fstream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#include <iostream>
#include <filesystem>

#include "Math/FTMath.h"
#include "Core/TemplateFunctions.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/CollisionManager.h"
#include "Scenes/Scene.h"
#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Renderer/Camera.h"

#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/BatchHeaders.h"

void ChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    SaveChunkData(ofs);
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ifs(fileName);
    LoadChunkData(ifs);
    CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
    ResourceManager::GetInstance()->LoadResources(ifs, FTCore::GetInstance());
    LoadActorsData(ifs);
    Camera::GetInstance()->LoadProperties(ifs);
}

void ChunkLoader::SaveChunkData(std::ofstream& out)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
    FileIOHelper::BeginDataPackSave(out, ChunkKeys::CHUNK_DATA);
    FileIOHelper::SaveInt(out, ChunkKeys::ACTOR_COUNT, currScene->GetActorCount());
    FileIOHelper::EndDataPackSave(out, ChunkKeys::CHUNK_DATA);
}

void ChunkLoader::SaveActorsData(std::ofstream& out)
{
}

void ChunkLoader::LoadActorsData(std::ifstream& ifs)
{
    Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
    std::pair<size_t, std::string>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::ACTOR_DATA);
    for (size_t i = 0; i < pack.first; ++i)
    {
        std::pair<size_t, std::string>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
        Actor* actor = DBG_NEW Actor(scene);
        actor->LoadProperties(ifs);
        actor->LoadComponents(ifs);
        actor->Initialize(FTCore::GetInstance());
        actor->Setup();
    }
}

void ChunkLoader::LoadChunkData(std::ifstream& ifs)
{
    int targetActor = 0;
    FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::CHUNK_DATA);
    FileIOHelper::LoadSize(ifs, mCurrentChunkData.ActorCount);
}

std::string ChunkLoader::GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix)
{
    return curr.substr(0, curr.length() - strlen(prevSuffix.c_str())) + postSuffix;
}

ChunkLoader::ChunkLoader()
    : mCurrentChunkData{}
{
    mComponentLoadMap =
    {
        {"AI",                &Component::Load<AI>},
        {"Animator",          &Component::Load<Animator>},
        {"BoxCollider2D",     &Component::Load<BoxCollider2D>},
        {"InputMove",         &Component::Load<InputMove>},
        {"Move",              &Component::Load<Move>},
        {"Rigidbody2D",       &Component::Load<Rigidbody2D>},
        {"SpriteRenderer",    &Component::Load<SpriteRenderer>},
        {"TileMap",           &Component::Load<TileMapRenderer>},
        {"TextRenderer",      &Component::Load<TextRenderer>},
        {"MeshRenderer",      &Component::Load<MeshRenderer>},

        {"GunFiringBehavior", &Component::Load<GunFiringBehavior>},
        {"BulletBehavior",    &Component::Load<BulletBehavior>},

        {"Seek",              &Component::Load<Seek>},
        {"Flee",              &Component::Load<Flee>}
    };
};

ChunkLoader::~ChunkLoader(){}