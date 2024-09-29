#include "EditorChunkLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Managers/ResourceManager.h"
#include "Math/FTMath.h"
#include "EditorLayer.h"
#include "Components/ComponentBatchHeaders.h"

EditorChunkLoader::EditorChunkLoader()
    :ChunkLoader()
{
    mComponentCreateMap =
    {
        {L"AIComponent",             &Component::Create<AIComponent>},
        {L"AnimatorComponent",       &Component::Create<AnimatorComponent>},
        {L"AnimSpriteComponent",     &Component::Create<SpriteAnimComponent>},
        {L"BGSpriteComponent",       &Component::Create<BGSpriteComponent>},
        {L"ColliderComponent",       &Component::Create<ColliderComponent>},
        {L"InputMoveComponent",      &Component::Create<InputMoveComponent>},
        {L"MoveComponent",           &Component::Create<MoveComponent>},
        {L"Rigidbody2DComponent",    &Component::Create<Rigidbody2DComponent>},
        {L"SpriteRendererComponent", &Component::Create<SpriteRendererComponent>},
        {L"TileMapComponent",        &Component::Create<TileMapComponent>},
        {L"SpriteAnimComponent",     &Component::Create<SpriteAnimComponent>},
        {L"GunFiringComponent",      &Component::Create<GunFiringComponent>},
        {L"MeshRendererComponent",   &Component::Create<MeshRendererComponent>},
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
    FileIOHelper::AddScalarValue<int>(out[SAVEKEY_ACTORCOUNT], EditorLayer::GetInstance()->GetEditorElements().size());
}

void EditorChunkLoader::SaveActorsData(nlohmann::ordered_json& out)
{
    Actor::ResetNextID();
    const std::vector<EditorElement*>& actors = EditorLayer::GetInstance()->GetEditorElements();
    for (size_t i = 0; i < actors.size(); ++i)
    {
        actors[i]->SaveProperties(out[i]);
        actors[i]->SaveComponents(out[i]["Components"]);
    }
}