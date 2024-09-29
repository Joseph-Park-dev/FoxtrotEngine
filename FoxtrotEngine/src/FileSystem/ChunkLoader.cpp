#include "FileSystem/ChunkLoader.h"

#include <string>
#include <fstream>
#include <Windows.h>
#include <commdlg.h>
#include <iostream>
#include <filesystem>

#include "EditorLayer.h"
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
    
}

void ChunkLoader::LoadChunk(const std::string fileName)
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
    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(ifs, currScene);
        currScene->AddActor(actor, actor->GetActorGroup());
    }
}

void ChunkLoader::LoadActorsToEditor(std::ifstream& ifs)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        Actor* tempActor = LoadIndividualActor(ifs, currScene);
        // Copies Actor Data to EditorElement.
        EditorLayer::GetInstance()->AddEditorElement(tempActor);
        // This prevents duplicated game objects.
        delete tempActor;
    }
}

//void ChunkLoader::LoadActors(std::ifstream& ifs)
//{
//    orderedJSONRef actorList = mJSON["ActorList"];
//    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
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
//    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
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
        {L"AIComponent",                &Component::Load<AIComponent>},
        {L"AnimatorComponent",          &Component::Load<AnimatorComponent>},
        {L"AnimSpriteComponent",        &Component::Load<SpriteAnimComponent>},
        {L"BGSpriteComponent",          &Component::Load<BGSpriteComponent>},
        {L"ColliderComponent",          &Component::Load<ColliderComponent>},
        {L"InputMoveComponent",         &Component::Load<InputMoveComponent>},
        {L"MoveComponent",              &Component::Load<MoveComponent>},
        {L"Rigidbody2DComponent",       &Component::Load<Rigidbody2DComponent>},
        {L"SpriteRendererComponent",    &Component::Load<SpriteRendererComponent>},
        {L"TileMapComponent",           &Component::Load<TileMapComponent>},
        {L"SpriteAnimComponent",        &Component::Load<SpriteAnimComponent>},
        {L"GunFiringComponent",         &Component::Load<GunFiringComponent>},
        {L"MeshRendererComponent",      &Component::Load<MeshRendererComponent>}
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

//
//void FileIOHelper::AddVector2(std::string key, FTVector2 value)
//{
//}
//
//void FileIOHelper::AddVector3(std::string key, FTVector3 value)
//{
//}
//
//
//void FileIOHelper::AddBasicString(std::string key, std::string value)
//{
//    //ofs.write((char*)&value[0], STRING_BUFFER_SIZE);
//    ++mUnmatched;
//}
//
//void FileIOHelper::AddWString(std::string key, std::wstring value)
//{
//    
//}
//
//void FileIOHelper::AddFloat(std::string key, float value)
//{
//}
//
//void FileIOHelper::AddInt(std::string key, int value)
//{
//    //ofs.write((char*)&value, sizeof(int));
//    ++mUnmatched;
//}
//
//void FileIOHelper::AddSize(std::string key, size_t value)
//{
//    //ofs.write((char*)&value, sizeof(size_t));
//    ++mUnmatched;

FTVector2 FileIOHelper::LoadVector2(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    float x = LoadFloat(ifs);
    float y = LoadFloat(ifs);
    return FTVector2(x, y);
}


std::string FileIOHelper::LoadBasicString(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    std::string str;
    std::getline(ifs, str);
    return str;
}

std::wstring FileIOHelper::LoadWString(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    std::string str;
    std::getline(ifs, str);
    return ToWString(str);
}

float FileIOHelper::LoadFloat(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    std::string buf;
    std::getline(ifs, buf);
    return std::stof(buf);
}

int FileIOHelper::LoadInt(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    std::string buf;
    std::getline(ifs, buf);
    return std::stoi(buf);
}

size_t FileIOHelper::LoadSize(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    std::string buf;
    std::getline(ifs, buf);
    return static_cast<size_t>(std::stoi(buf));
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, FTVector2& value)
{
    assert(mUnmatched > 0);
    float x = 0.f; float y = 0.f;
    LoadFloat(ifs, x);
    LoadFloat(ifs, y);
    value = FTVector2(x, y);
}

void FileIOHelper::LoadBasicString(std::ifstream& ifs, std::string& value)
{
    assert(mUnmatched > 0);
    value = LoadBasicString(ifs);
}

void FileIOHelper::LoadWString(std::ifstream& ifs, std::wstring& value)
{
    assert(mUnmatched > 0);
    value = LoadWString(ifs);
}

void FileIOHelper::LoadFloat(std::ifstream& ifs, float& value)
{
    assert(mUnmatched > 0);
    value = LoadFloat(ifs);
}

void FileIOHelper::LoadInt(std::ifstream& ifs, int& value)
{
    assert(mUnmatched > 0);
    value = LoadInt(ifs);
}

void FileIOHelper::LoadSize(std::ifstream& ifs, size_t& value)
{
    assert(mUnmatched > 0);
    value = LoadSize(ifs);
}

void FileIOHelper::SaveFileIOData(std::string filename)
{
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    if (file)
    {
        file.write((char*)&mUnmatched, sizeof(int));
        file.close();
    }
}

void FileIOHelper::LoadFileIOData(std::string filename)
{
    std::ifstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    if (file)
    {
        file.read((char*)&mUnmatched, sizeof(int));
        file.close();
    }
}

//void SaveWString(const std::wstring& str, FILE* file)
//{
//    // Data serialization (데이터 직렬화)
//    const wchar_t* name = str.c_str();
//    size_t len = str.length();
//
//    fwrite(&len, sizeof(size_t), 1, file);
//    fwrite(name, sizeof(wchar_t), len, file);
//    SDL_Log("Saved : %ls", name);
//}
//
//void LoadWString(std::wstring& str, FILE* file)
//{
//    size_t len = 0;
//    fread(&len, sizeof(size_t), 1, file);
//    wchar_t szBuff[256] = {};
//    fread(szBuff, sizeof(wchar_t), len, file);
//    str = szBuff;
//    SDL_Log("Loaded : %ls", str.c_str());
//}
//
//void LoadDataFromFile(char* dst, FILE* file)
//{
//    int i = 0;
//    while (true)
//    {
//        char c = (char)getc(file);
//        if (c == '\n')
//        {
//            dst[i++] = '\0';
//            break;
//        }
//
//        dst[i++] = c;
//    }
//}