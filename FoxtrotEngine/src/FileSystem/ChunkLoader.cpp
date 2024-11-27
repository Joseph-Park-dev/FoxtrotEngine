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
#include "Actors/UIs/PanelUI.h"
#include "Actors/Backgrounds/BackgroundActor.h"

#include "Components/ComponentBatchHeaders.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

void ChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    SaveChunkData(ofs, SceneManager::GetInstance()->GetCurrentScene());
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ofs(fileName);
}

void ChunkLoader::SaveChunkData(std::ofstream& out, Scene* currScene)
{
    FileIOHelper::BeginDataPackSave(out, ChunkKeys::CHUNK_DATA);
    FileIOHelper::SaveInt(out, ChunkKeys::TARGET_ACTOR, 1);
    FileIOHelper::SaveInt(out, ChunkKeys::ACTOR_COUNT, currScene->GetActorCount());
    FileIOHelper::EndDataPackSave(out, ChunkKeys::CHUNK_DATA);
}

void ChunkLoader::SaveActorsData(std::ofstream& out)
{
}

void ChunkLoader::LoadChunkData(std::ifstream& ifs, Scene* currScene)
{
    int targetActor = 0;
    FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::CHUNK_DATA);
    FileIOHelper::LoadSize(ifs, mCurrentChunkData.ActorCount);
    FileIOHelper::LoadInt(ifs, targetActor);
}

void ChunkLoader::LoadActors(std::ifstream& ifs)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(ifs, currScene);
        currScene->AddActor(actor, actor->GetActorGroup());
    }
}

Actor* ChunkLoader::LoadIndividualActor(std::ifstream& ifs, Scene* currScene)
{
    Actor* actor = DBG_NEW Actor(currScene);
    actor->LoadProperties(ifs);
    actor->LoadComponents(ifs);
    return actor;
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
        {"GunFiringComponent",         &Component::Load<GunFiringComponent>},
        {"MeshRendererComponent",      &Component::Load<MeshRendererComponent>}
    };
};

ChunkLoader::~ChunkLoader(){}