#include "EditorChunkLoader.h"

#include <fstream>

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Math/FTMath.h"
#include "Components/ComponentBatchHeaders.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

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
        {"BoxCollider2DComponent",  &Component::Create<BoxCollider2DComponent>},
        {"InputMoveComponent",      &Component::Create<InputMoveComponent>},
        {"MoveComponent",           &Component::Create<MoveComponent>},
        {"Rigidbody2DComponent",    &Component::Create<Rigidbody2DComponent>},
        {"SpriteRendererComponent", &Component::Create<SpriteRendererComponent>},
        {"TileMapComponent",        &Component::Create<TileMapComponent>},
        {"SpriteAnimComponent",     &Component::Create<SpriteAnimComponent>},
        {"MeshRendererComponent",   &Component::Create<MeshRendererComponent>},

        // UI related
        {"ButtonUIComponent",       &Component::Create<ButtonUIComponent>},
        {"PanelUIComponent",        &Component::Create<PanelUIComponent>},

        // Actor Behaviors
        {"GunFiringBehavior",       &Component::Create<GunFiringBehavior>},
        {"BulletBehavior",          &Component::Create<BulletBehavior>}
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
    ResourceManager::GetInstance()->LoadResources(ifs, FTCoreEditor::GetInstance());
    LoadActorsData(ifs);
}

void EditorChunkLoader::SaveActorsData(std::ofstream& ofs)
{
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    std::vector<Actor*>* actors = scene->GetActors();
    FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::ACTOR_DATA);
    
    for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
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
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::ACTOR_DATA);
}

void EditorChunkLoader::LoadActorsData(std::ifstream& ifs)
{
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    std::pair<size_t, std::string>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::ACTOR_DATA);
    for (size_t i = 0; i < pack.first; ++i)
    {
        std::pair<size_t, std::string>&& actorData = FileIOHelper::BeginDataPackLoad(ifs);
        EditorElement* element = DBG_NEW EditorElement(scene);
        element->LoadProperties(ifs);
        element->LoadComponents(ifs);
        element->Initialize(FTCoreEditor::GetInstance());
    }
}