#include "FileSystem/ChunkLoader.h"

#include <string>
#include <fstream>
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

void ChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    int count = FileIOHelper::BeginDataPackSave(ofs,)
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ofs(fileName);

}

void ChunkLoader::SaveChunkData(std::fstream& out)
{
}

void ChunkLoader::SaveActorsData(std::fstream& out)
{
}

//void ChunkLoader::LoadChunkData(YAML::Node& node)
//{
//    /*
//    ChunkData save order
//        1) Camera2D values
//            a) TargetActorID    -> int
//            a) RenderResolution -> FTVector2
//            b) ScreenCenter     -> FTVector2
//        2) Number of Actors     -> int
//    */
//
//    /*Camera::GetInstance()->SetTargetActorID(mCurrentChunkData.TargetActorID);
//    Camera::GetInstance()->SetRenderResolution(mCurrentChunkData.RenderResolution);
//    Camera::GetInstance()->SetScreenCenter(mCurrentChunkData.ScreenCenter);*/
//}

void ChunkLoader::LoadActors(std::ifstream& ifs)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(ifs, currScene);
        currScene->AddActor(actor, actor->GetActorGroup());
    }
}

void ChunkLoader::LoadActors(nlohmann::ordered_json& in)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(in, currScene);
        currScene->AddActor(actor, actor->GetActorGroup());
    }
}

//void ChunkLoader::LoadActors(std::ifstream& ifs)
//{
//    orderedJSONRef actorList = mJSON["ActorList"];
//    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
//
//    size_t actorCount = actorList["Count"];
//    for (size_t i = 0; i < actorCount; ++i)
//    {
//        orderedJSONRef actorTree = actorList[std::to_string(i)];
//        Actor* actor = LoadIndividualActor(currScene, actorTree);
//        currScene->AddActor(actor, actor->GetActorGroup());
//    }
//}
//
//void ChunkLoader::LoadActorsToEditor(std::ifstream& ifs)
//{
//    orderedJSONRef actorList = mJSON["ActorList"];
//    Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
//
//    size_t actorCount = actorList["Count"];
//    for (size_t i = 0; i < actorCount; ++i)
//    {
//        orderedJSONRef actorTree = actorList[std::to_string(i)];
//        Actor* actor = LoadIndividualActor(currScene, actorTree);
//        EditorLayer::GetInstance()->AddEditorElement(actor);
//    }
//}
//
Actor* ChunkLoader::LoadIndividualActor(std::ifstream& ifs, Scene* currScene)
{
    Actor* actor = new Actor(currScene);
    actor->LoadProperties(ifs);
    actor->LoadComponents(ifs);
    return actor;
}

Actor* ChunkLoader::LoadIndividualActor(nlohmann::ordered_json& in, Scene* currScene)
{
    Actor* actor = new Actor(currScene);
    actor->LoadProperties(in);
    actor->LoadComponents(in["Components"]);
    return actor;
}

std::string ChunkLoader::GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix)
{
    return curr.substr(0, curr.length() - strlen(prevSuffix.c_str())) + postSuffix;
}
 // _DEBUG

ChunkLoader::ChunkLoader()
    : mCurrentChunkData{}
{
    mComponentLoadMap =
    {
        {"AIComponent",                &Component::Load<AIComponent>},
        {"AnimatorComponent",          &Component::Load<AnimatorComponent>},
        {"BGSpriteComponent",          &Component::Load<BGSpriteComponent>},
        {"ColliderComponent",          &Component::Load<ColliderComponent>},
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

void FileIOHelper::AddVector2(nlohmann::ordered_json& json, FTVector2 value)
{
    json["x"] = value.x;
    json["y"] = value.y;
}
void FileIOHelper::AddVector3(nlohmann::ordered_json& json, FTVector3 value)
{
    json["x"] = value.x;
    json["y"] = value.y;
    json["z"] = value.z;
}

FTVector2 FileIOHelper::LoadVector2(nlohmann::ordered_json& json, std::string key)
{
    FTVector2 value = FTVector2::Zero;
    value.x = json[key]["x"];
    value.y = json[key]["y"];
    return value;
}

FTVector3 FileIOHelper::LoadVector3(nlohmann::ordered_json& json, std::string key)
{
    FTVector3 value = FTVector3::Zero;
    value.x = json[key]["x"];
    value.y = json[key]["y"];
    value.z = json[key]["z"];
    return value;
}

int FileIOHelper::BeginDataPackLoad(std::ifstream& ifs)
{
    std::string dataPackKey;
    std::getline(ifs, dataPackKey, '\n');

    // Parse data pack key 
    std::string name = ExtractUntil(dataPackKey, '<');
    int	count = std::stoi(GetBracketedVal(dataPackKey, '<', '>'));

    std::cout << "Parsing data pack : " << name << '\n';
    std::cout << name << '\n';

    return count;
}

void FileIOHelper::ParseVector3(std::string& line, FTVector3& arg)
{
    line = GetBracketedVal(line, '(', ')');
    float x = std::stof(ExtractUntil(line, ','));
    float y = std::stof(ExtractUntil(line, ','));
    float z = std::stof(ExtractUntil(line, ')'));

    arg = FTVector3(x, y, z);
}

void FileIOHelper::ParseVector2(std::string& line, FTVector2& arg)
{
    line = GetBracketedVal(line, '(', ')');
    float x = std::stof(ExtractUntil(line, ','));
    float y = std::stof(ExtractUntil(line, ','));

    arg = FTVector2(x, y);
}

void FileIOHelper::ParseInt(std::string& line, int& arg)
{
    line = GetBracketedVal(line, '(', ')');
    arg = std::stoi(line);
}

void FileIOHelper::ParseFloat(std::string& line, float& arg)
{
    line = GetBracketedVal(line, '(', ')');
    arg = std::stof(line);
}

void FileIOHelper::ParseString(std::string& line, std::string& arg)
{
    line = GetBracketedVal(line, '(', ')');
    arg.assign(line);
}

int FileIOHelper::BeginDataPackSave(std::ofstream& ofs, const std::string dataPackKey, const int varCount)
{
    ofs << dataPackKey << "<" << std::to_string(varCount) << ">" << std::endl;
    std::cout << "Saving data pack : " << dataPackKey << '\n';
    return varCount;
}

void FileIOHelper::SaveVector3(std::ofstream& ofs, const std::string valName, const FTVector3& vec3)
{
    ofs << valName << "[Vector3]" << std::endl;
    ofs << vec3 << std::endl;
}

void FileIOHelper::SaveVector2(std::ofstream& ofs, const std::string valName, const FTVector2& vec2)
{
    ofs << valName << "[Vector2]" << std::endl;
    ofs << vec2 << std::endl;
}

void FileIOHelper::SaveInt(std::ofstream& ofs, const std::string valName, const int& intVal)
{
    ofs << valName << "[int]" << std::endl;
    ofs << std::to_string(intVal) << std::endl;
}

void FileIOHelper::SaveFloat(std::ofstream& ofs, const std::string valName, const float& floatVal)
{
    ofs << valName << "[float]" << std::endl;
    ofs << std::to_string(floatVal) << std::endl;
}

void FileIOHelper::SaveString(std::ofstream& ofs, const std::string valName, const std::string& strVal)
{
    ofs << valName << "[string]" << std::endl;
    ofs << strVal << std::endl;
}

std::string FileIOHelper::ExtractUntil(std::string& line, const char end) {
    size_t typeBeg = line.find(end);
    std::string result = line.substr(0, typeBeg);

    // Erase the extracted value from line, including end character.
    line.erase(0, typeBeg + 1);
    return result;
}

std::string FileIOHelper::GetBracketedVal(std::string& str, const char left, const char right) {
    size_t begin = str.find(left);
    size_t end = str.find(right);
    return str.substr(begin + 1, end - (begin + 1));
}

