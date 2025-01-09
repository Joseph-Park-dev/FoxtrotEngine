// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorChunkLoader.h"

#include <fstream>

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Managers/CollisionManager.h"
#include "Math/FTMath.h"
#include "Components/ComponentBatchHeaders.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"

EditorChunkLoader::EditorChunkLoader()
    :ChunkLoader()
{
    // You must list all Components to be used during runtime.
    // That includes the user defined Components, or "Behavior"s.
    mComponentCreateMap =
    {
        {"AIComponent",             &Component::Create<AIComponent>},
        {"AnimatorComponent",       &Component::Create<AnimatorComponent>},
        {"BoxCollider2DComponent",  &Component::Create<BoxCollider2DComponent>},
        {"InputMoveComponent",      &Component::Create<InputMoveComponent>},
        {"MoveComponent",           &Component::Create<MoveComponent>},
        {"Rigidbody2DComponent",    &Component::Create<Rigidbody2DComponent>},
        {"SpriteRendererComponent", &Component::Create<SpriteRendererComponent>},
        {"TileMapComponent",        &Component::Create<TileMapComponent>},
        {"MeshRendererComponent",   &Component::Create<MeshRendererComponent>},

        // UI related
        {"ButtonUIComponent",       &Component::Create<ButtonUIComponent>},
        {"PanelUIComponent",        &Component::Create<PanelUIComponent>},

        // Actor Behaviors
        {"GunFiringBehavior",       &Component::Create<GunFiringBehavior>},
        {"BulletBehavior",          &Component::Create<BulletBehavior>},
        {"Seek",                    &Component::Create<Seek>}
    };
}

EditorChunkLoader::~EditorChunkLoader() {}

void EditorChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    // Save -> ActorData comes first, // Load -> ChunkData comes first
    Camera::GetInstance()->SaveProperties(ofs);
    SaveActorsData(ofs);
    ResourceManager::GetInstance()->SaveResources(ofs);
    CollisionManager::GetInstance()->SaveCollisionMarks(ofs);
    SaveChunkData(ofs);
    FileIOHelper::SaveBufferToFile(ofs);
}

void EditorChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ifs(fileName);
    LoadChunkData(ifs);
    CollisionManager::GetInstance()->LoadCollisionMarks(ifs);
    ResourceManager::GetInstance()->LoadResources(ifs, FTCoreEditor::GetInstance());
    LoadActorsData(ifs);
    Camera::GetInstance()->LoadProperties(ifs);
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
        Actor actor = Actor();
        actor.LoadProperties(ifs);
        actor.LoadComponents(ifs);

        EditorElement* element = DBG_NEW EditorElement(&actor, scene);
        element->Initialize(FTCoreEditor::GetInstance());
    }
}