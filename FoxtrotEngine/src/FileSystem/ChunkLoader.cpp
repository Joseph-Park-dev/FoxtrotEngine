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

void ChunkLoader::SaveChunk(const std::string fileName)
{
    std::ofstream ofs(fileName);
    SaveChunkData(ofs);
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    std::ifstream ofs(fileName);

}

void ChunkLoader::SaveChunkData(std::ofstream& out)
{
    FileIOHelper::BeginDataPackSave(out, ChunkKeys::CHUNK_DATA);
    FileIOHelper::SaveInt(out, ChunkKeys::TARGET_ACTOR, 1);
    FileIOHelper::SaveVector2(out, ChunkKeys::RENDER_RESOLUTION, FTVector2::Zero);
    FileIOHelper::SaveVector2(out, ChunkKeys::RENDER_SCREENCENTER, FTVector2(123, 123));
    Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
    FileIOHelper::SaveInt(out, ChunkKeys::ACTOR_COUNT, scene->GetActorCount());
    FileIOHelper::EndDataPackSave(out, ChunkKeys::CHUNK_DATA);
}

void ChunkLoader::SaveActorsData(std::ofstream& out)
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
    actor->LoadComponents(ifs, FTCore::GetInstance());
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
        {"BoxColliderComponent",       &Component::Load<BoxColliderComponent>},
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

std::list<std::string> FileIOHelper::mDataBuffer = {};
std::list<int> FileIOHelper::mItemCounts = {};
std::list<std::string> FileIOHelper::mCurrentDataPack = {};
int FileIOHelper::mDataPackIdent = 0;
std::string FileIOHelper::mItemIdent = std::string(mDataPackIdent, '\t');

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

// This function is less safe since there is no assert to check data pack name
std::pair<size_t, std::string> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs)
{
    std::string loadedDataPackKey;
    std::getline(ifs, loadedDataPackKey, '\n');
    LTrim(loadedDataPackKey);

    // Parse data pack key 
    std::string name = ExtractUntil(loadedDataPackKey, '<');
    size_t count = std::stoi(GetBracketedVal(loadedDataPackKey, '<', '>'));

    std::cout << "Parsing data pack : " << name << '\n';
    std::cout << name << '\n';

    std::pair<size_t, std::string> result{ count, name };
    return result;
}

std::pair<size_t, std::string> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs, std::string dataPackKey)
{
    std::string loadedDataPackKey;
    std::getline(ifs, loadedDataPackKey, '\n');
    LTrim(loadedDataPackKey);

    // Parse data pack key 
    std::string name = ExtractUntil(loadedDataPackKey, '<');
    size_t count = std::stoi(GetBracketedVal(loadedDataPackKey, '<', '>'));

    assert(name == dataPackKey);

    std::cout << "Parsing data pack : " << name << '\n';
    std::cout << name << '\n';

    std::pair<size_t, std::string> result{ count, name };
    return result;
}

void FileIOHelper::LoadInt(std::ifstream& ifs, int& intVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseInt(line, intVal);
}

void FileIOHelper::LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseUnsignedInt(line, intVal);
    LogInt(intVal);
}

void FileIOHelper::LoadFloat(std::ifstream& ifs, float& floatVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseFloat(line, floatVal);
}

void FileIOHelper::LoadBasicString(std::ifstream& ifs, std::string& strVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseString(line, strVal);
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, FTVector2& vec2)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector3(std::ifstream& ifs, FTVector3& vec3)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseVector3(line, vec3);
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
    LTrim(line);
    arg = std::stoi(line);
}

void FileIOHelper::ParseUnsignedInt(std::string& line, unsigned int& arg)
{
    LTrim(line);
    arg = std::stoul(line, nullptr, 0);
}

void FileIOHelper::ParseFloat(std::string& line, float& arg)
{
    LTrim(line);
    arg = std::stof(line);
}

void FileIOHelper::ParseString(std::string& line, std::string& arg)
{
    LTrim(line);
    arg.assign(line);
}

void FileIOHelper::BeginDataPackSave(std::ofstream& ofs, std::string dataPackKey)
{
    mDataPackIdent = mItemCounts.size();
    mItemIdent = std::string(mDataPackIdent + 1, '\t');

    if (0 < mItemCounts.size()) 
    {
        ++mItemCounts.back();
    }
    mItemCounts.push_back(0);
    mCurrentDataPack.push_back(dataPackKey);
    
    std::cout << "Saving data pack : " << dataPackKey << '\n';
}

void FileIOHelper::EndDataPackSave(std::ofstream& ofs, std::string dataPackKey)
{
    assert(mCurrentDataPack.back() == dataPackKey);

    int itemCount = mItemCounts.back();
    mItemCounts.pop_back();
    mCurrentDataPack.pop_back();
    mDataPackIdent = mItemCounts.size();
    mItemIdent = std::string(mDataPackIdent + 1, '\t');

    // Returning to the datapack ident level
    std::string title = std::string(mDataPackIdent, '\t') + dataPackKey + "<" + std::to_string(itemCount) + ">";
    mDataBuffer.push_back(title);
}

void FileIOHelper::SaveBufferToFile(std::ofstream& ofs)
{
    // Key & Value pairs = size() * 2
    std::list<std::string>::reverse_iterator iter = mDataBuffer.rbegin();
    for (; iter != mDataBuffer.rend(); ++iter) {
        ofs << *iter << '\n';
    }
    mDataBuffer.clear();
}

void FileIOHelper::SaveVector3(std::ofstream& ofs, const std::string valName, const FTVector3& vec3)
{
    std::string itemTitle =  mItemIdent + valName + "[Vector3]" + '\n';
    std::string item = mItemIdent + "(" + std::to_string(vec3.x) + "," + std::to_string(vec3.y) + "," + std::to_string(vec3.z) + ")";
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveVector2(std::ofstream& ofs, const std::string valName, const FTVector2& vec2)
{
    std::string itemTitle = mItemIdent + valName + "[Vector2]" + '\n';
    std::string item = mItemIdent + "(" + std::to_string(vec2.x) + "," + std::to_string(vec2.y) + ")";
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveInt(std::ofstream& ofs, const std::string valName, const int& intVal)
{
    std::string itemTitle = mItemIdent + valName + "[int]" + '\n';
    std::string item = mItemIdent + std::to_string(intVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveUnsignedInt(std::ofstream& ofs, const std::string valName, const unsigned int& intVal)
{
    std::string itemTitle = mItemIdent + valName + "[unsigned int]" + '\n';
    std::string item = mItemIdent + std::to_string(intVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveFloat(std::ofstream& ofs, const std::string valName, const float& floatVal)
{
    std::string itemTitle = mItemIdent + valName + "[float]" + '\n';
    std::string item = mItemIdent + std::to_string(floatVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveString(std::ofstream& ofs, const std::string valName, const std::string& strVal)
{
    std::string itemTitle = mItemIdent + valName + "[string]" + '\n';
    std::string item = mItemIdent + strVal;
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
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