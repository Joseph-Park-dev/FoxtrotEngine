#include "EditorChunkLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Math/FTMath.h"
#include "Components/ComponentBatchHeaders.h"
#include "EditorLayer.h"
#include "EditorSceneManager.h"

EditorChunkLoader::EditorChunkLoader()
    :ChunkLoader()
{
    mComponentCreateMap =
    {
        {L"AIComponent",             &EditorElement::Create<AIComponent>},
        {L"AnimatorComponent",       &EditorElement::Create<AnimatorComponent>},
        {L"AnimSpriteComponent",     &EditorElement::Create<SpriteAnimComponent>},
        {L"BGSpriteComponent",       &EditorElement::Create<BGSpriteComponent>},
        {L"ColliderComponent",       &EditorElement::Create<ColliderComponent>},
        {L"InputMoveComponent",      &EditorElement::Create<InputMoveComponent>},
        {L"MoveComponent",           &EditorElement::Create<MoveComponent>},
        {L"Rigidbody2DComponent",    &EditorElement::Create<Rigidbody2DComponent>},
        {L"SpriteRendererComponent", &EditorElement::Create<SpriteRendererComponent>},
        {L"TileMapComponent",        &EditorElement::Create<TileMapComponent>},
        {L"SpriteAnimComponent",     &EditorElement::Create<SpriteAnimComponent>},
        {L"GunFiringComponent",      &EditorElement::Create<GunFiringComponent>},
        {L"MeshRendererComponent",   &EditorElement::Create<MeshRendererComponent>},
    };
}

EditorChunkLoader::~EditorChunkLoader() {}

void EditorChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    nlohmann::ordered_json out;
    SaveChunkData(out[MAPKEY_CHUNKDATA]);
    SaveActorsData(out[MAPKEY_ACTORDATA]);
    ResourceManager::GetInstance()->SaveResources(out[MAPKEY_RESOURCEDATA]);
    ofs << std::setw(4) << out << std::endl;
}

void EditorChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ifs(fileName);
    nlohmann::ordered_json in = nlohmann::ordered_json::parse(ifs);
    ResourceManager::GetInstance()->LoadResources(in["ResourceData"]);
}

void EditorChunkLoader::SaveChunkData(nlohmann::ordered_json& out)
{
    FileIOHelper::AddScalarValue<int>(out[SAVEKEY_TARGETACTORID], 1);
    FileIOHelper::AddVector2(out[SAVEKEY_RENDERRES], FTVector2::Zero);
    FileIOHelper::AddVector2(out[SAVEKEY_SCREENCENTER], FTVector2(123, 123));
    EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
    FileIOHelper::AddScalarValue<int>(out[SAVEKEY_ACTORCOUNT], scene->GetActorCount());
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
            element->SaveProperties(out[j]);
            element->SaveComponents(out[j]["Components"]);
        }
    }
}