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
        {"AIComponent",             &EditorElement::Create<AIComponent>},
        {"AnimatorComponent",       &EditorElement::Create<AnimatorComponent>},
        {"AnimSpriteComponent",     &EditorElement::Create<SpriteAnimComponent>},
        {"BGSpriteComponent",       &EditorElement::Create<BGSpriteComponent>},
        {"ColliderComponent",       &EditorElement::Create<ColliderComponent>},
        {"InputMoveComponent",      &EditorElement::Create<InputMoveComponent>},
        {"MoveComponent",           &EditorElement::Create<MoveComponent>},
        {"Rigidbody2DComponent",    &EditorElement::Create<Rigidbody2DComponent>},
        {"SpriteRendererComponent", &EditorElement::Create<SpriteRendererComponent>},
        {"TileMapComponent",        &EditorElement::Create<TileMapComponent>},
        {"SpriteAnimComponent",     &EditorElement::Create<SpriteAnimComponent>},
        {"GunFiringComponent",      &EditorElement::Create<GunFiringComponent>},
        {"MeshRendererComponent",   &EditorElement::Create<MeshRendererComponent>},
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
    //ResourceManager::GetInstance()->LoadResources(in["ResourceData"]);
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
            element->SaveProperties(ofs);
            element->SaveComponents(ofs);
            FileIOHelper::EndDataPackSave(ofs, element->GetName());
        }
    }
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES);
}
