#include "EditorChunkLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Math/FTMath.h"
#include "Components/ComponentBatchHeaders.h"
#include "FileSystem/ChunkFileKeys.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"

EditorChunkLoader::EditorChunkLoader()
    :ChunkLoader()
{
    mComponentCreateMap =
    {
        {"AIComponent",             &Component::Create<AIComponent>},
        {"AnimatorComponent",       &Component::Create<AnimatorComponent>},
        {"BGSpriteComponent",       &Component::Create<BGSpriteComponent>},
        {"ColliderComponent",       &Component::Create<ColliderComponent>},
        {"InputMoveComponent",      &Component::Create<InputMoveComponent>},
        {"MoveComponent",           &Component::Create<MoveComponent>},
        {"Rigidbody2DComponent",    &Component::Create<Rigidbody2DComponent>},
        {"SpriteRendererComponent", &Component::Create<SpriteRendererComponent>},
        {"TileMapComponent",        &Component::Create<TileMapComponent>},
        {"SpriteAnimComponent",     &Component::Create<SpriteAnimComponent>},
        {"GunFiringComponent",      &Component::Create<GunFiringComponent>},
        {"MeshRendererComponent",   &Component::Create<MeshRendererComponent>},
    };
}

EditorChunkLoader::~EditorChunkLoader() {}

void EditorChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    // Save -> ActorData comes first, // Load -> ChunkData comes first
    SaveActorsData(ofs);
    ResourceManager::GetInstance()->SaveResources(ofs);
    SaveChunkData(ofs);
    FileIOHelper::SaveBufferToFile(ofs);
}

void EditorChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ifs(fileName);
    LoadChunkData(ifs);
    ResourceManager::GetInstance()->LoadResources(ifs);
    LoadActorsData(ifs);
}

void EditorChunkLoader::SaveChunkData(nlohmann::ordered_json& out)
{
    FileIOHelper::AddScalarValue<int>(out[ChunkKeys::TARGET_ACTOR], 1);
    FileIOHelper::AddVector2(out[ChunkKeys::RENDER_RESOLUTION], FTVector2::Zero);
    FileIOHelper::AddVector2(out[ChunkKeys::RENDER_SCREENCENTER], FTVector2(123, 123));
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    FileIOHelper::AddScalarValue<int>(out[ChunkKeys::ACTOR_COUNT], scene->GetActorCount());
}

void EditorChunkLoader::SaveActorsData(nlohmann::ordered_json& out)
{
    Actor::ResetNextID();
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    std::vector<Actor*>* actors = scene->GetActors();
    for (size_t i = 0; i < (size_t)ActorGroup::END; ++i) 
    {
        for (size_t j = 0; j < actors[i].size(); ++j)
        {
            EditorElement* element = dynamic_cast<EditorElement*>(actors[i][j]);
            size_t index = (size_t)ActorGroup::END * i + j;
            element->SaveProperties(out[index]);
            element->SaveComponents(out[index]["Components"]);
        }
    }
}

void EditorChunkLoader::SaveChunkData(std::ofstream& ofs)
{
    FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::CHUNK_DATA);
    FileIOHelper::SaveInt(ofs, ChunkKeys::TARGET_ACTOR, 1);
    FileIOHelper::SaveVector2(ofs, ChunkKeys::RENDER_RESOLUTION, FTVector2::Zero);
    FileIOHelper::SaveVector2(ofs, ChunkKeys::RENDER_SCREENCENTER, FTVector2(123, 123));
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    FileIOHelper::SaveInt(ofs, ChunkKeys::ACTOR_COUNT, scene->GetActorCount());
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::CHUNK_DATA);
}

void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
{
    Actor::ResetNextID();
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    std::vector<Actor*>* actors = scene->GetActors();
    FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES);
    for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
    {
        for (size_t j = 0; j < actors[i].size(); ++j)
        {
            EditorElement* element = dynamic_cast<EditorElement*>(actors[i][j]);
            size_t index = (size_t)ActorGroup::END * i + j;
            FileIOHelper::BeginDataPackSave(ofs, element->GetName());
            element->SaveComponents(ofs);
            element->SaveProperties(ofs);
            FileIOHelper::EndDataPackSave(ofs, element->GetName());
        }
    }
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES);
}

void EditorChunkLoader::LoadChunkData(std::ifstream& ifs)
{
    int targetActor = 0;
    FTVector2 res = FTVector2::Zero;
    FTVector2 center = FTVector2::Zero;
    int actorCount = 0;

    FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::CHUNK_DATA);
    FileIOHelper::LoadInt(ifs, targetActor);
    FileIOHelper::LoadVector2(ifs, res);
    FileIOHelper::LoadVector2(ifs, center);
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    FileIOHelper::LoadInt(ifs, actorCount);

    LogInt(targetActor);
    LogVector2(res);
    LogVector2(center);
    LogInt(actorCount);
}

void EditorChunkLoader::LoadActorsData(std::ifstream& ifs)
{
    Actor::ResetNextID();
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    std::pair<size_t, std::string>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::ACTOR_PROPERTIES);
    for (size_t i = 0; i < pack.first; ++i)
    {
        FileIOHelper::BeginDataPackLoad(ifs);
        EditorElement* element = new EditorElement(scene);
        element->LoadProperties(ifs);
        element->LoadComponents(ifs, FTCoreEditor::GetInstance());
    }
}
