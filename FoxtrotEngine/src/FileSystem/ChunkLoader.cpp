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
#include "Scenes/Scene.h"
#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Actors/Backgrounds/BackgroundActor.h"

#include "Components/ComponentBatchHeaders.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

void ChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    SaveChunkData(ofs);
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ifs(fileName);
    LoadChunkData(ifs);
    ResourceManager::GetInstance()->LoadResources(ifs, FTCore::GetInstance());
    LoadActorsData(ifs);
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
        FileIOHelper::BeginDataPackLoad(ifs);
        Actor* actor = DBG_NEW Actor(scene);
        actor->LoadProperties(ifs);
        actor->LoadComponents(ifs);
        actor->Initialize(FTCore::GetInstance());
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
        {"AIComponent",                &Component::Load<AIComponent>},
        {"AnimatorComponent",          &Component::Load<AnimatorComponent>},
        {"BGSpriteComponent",          &Component::Load<BGSpriteComponent>},
        {"BoxCollider2DComponent",       &Component::Load<BoxCollider2DComponent>},
        {"InputMoveComponent",         &Component::Load<InputMoveComponent>},
        {"MoveComponent",              &Component::Load<MoveComponent>},
        {"Rigidbody2DComponent",       &Component::Load<Rigidbody2DComponent>},
        {"SpriteRendererComponent",    &Component::Load<SpriteRendererComponent>},
        {"TileMapComponent",           &Component::Load<TileMapComponent>},
        {"SpriteAnimComponent",        &Component::Load<SpriteAnimComponent>},
        {"MeshRendererComponent",      &Component::Load<MeshRendererComponent>},

        {"GunFiringBehavior",         &Component::Load<GunFiringBehavior>},
        {"BulletBehavior",         &Component::Load<BulletBehavior>},
    };
};

ChunkLoader::~ChunkLoader(){}