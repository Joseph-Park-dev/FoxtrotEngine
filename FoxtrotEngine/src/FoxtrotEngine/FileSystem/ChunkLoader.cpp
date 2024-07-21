#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

#include <string>
#include <fstream>
#include <Windows.h>
#include <commdlg.h>
#include <iostream>

#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Managers/SceneManager.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Scenes/Scene.h"
#include "FoxtrotEngine/Actors/ActorGroup.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/UIs/PanelUI.h"
#include "FoxtrotEngine/Actors/Backgrounds/BackgroundActor.h"
#include "FoxtrotEngine/Renderer/Camera2D.h"
#include "FoxtrotEngine/Components/ComponentBatchHeaders.h"

#include "Scripts/ActorList/Ship.h"
#include "Scripts/ActorList/Asteroid.h"
#include "Scripts/ActorList/GroundObject.h"

#ifdef _DEBUG
#include "FoxtrotEditor/EditorLayer.h"
#include "FoxtrotEditor/EditorCamera2D.h"
#endif // _DEBUG

#ifdef _DEBUG
void ChunkLoader::SaveChunk(const std::string fileName)
{
    // Serialization into binary
    std::ofstream file;
    file.open(fileName, std::ios::binary);
    assert(file);
    if (file.is_open())
    {
        FileIOHelper::ResetFileIOData();
        SaveChunkData(file);
        SaveActors(file);
        file.close();
    }
    FileIOHelper::SaveFileIOData(GetConvertedFileName(fileName, CHUNK_FORMAT, FILE_IO_FORMAT));
}

void ChunkLoader::SaveChunkData(std::ofstream& ofs)
{   
    /*
    ChunkData save order
        1) Camera2D values
            a) TargetActorID    -> int
            a) RenderResolution -> FTVector2
            b) ScreenCenter     -> FTVector2
        2) Number of Actors     -> int
    */
    // Saves Camera2D Values.
    mCurrentChunkData.TargetActorID = Camera2D::GetInstance()->GetTargetActorID();
    mCurrentChunkData.RenderResolution = Camera2D::GetInstance()->GetRenderResolution();
    mCurrentChunkData.ScreenCenter = Camera2D::GetInstance()->GetScreenCenter();
    mCurrentChunkData.ActorCount = EditorLayer::GetInstance()->GetEditorElements().size();

    FileIOHelper::AddInt     (ofs, mCurrentChunkData.TargetActorID);
    FileIOHelper::AddVector2 (ofs, mCurrentChunkData.RenderResolution);
    FileIOHelper::AddVector2 (ofs, mCurrentChunkData.ScreenCenter);
    FileIOHelper::AddInt     (ofs, mCurrentChunkData.ActorCount);
}

void ChunkLoader::SaveResources(std::ofstream& of)
{
    std::unordered_map<std::string, FTTexture*>::const_iterator iter;
    std::unordered_map<std::string, FTTexture*>& texMap = ResourceManager::GetInstance()->GetTexturesMap();
    for (iter = texMap.begin(); iter != texMap.end(); ++iter) {

    }
}

void ChunkLoader::SaveActors(std::ofstream& ofs)
{
    Actor::ResetNextID();
    const std::vector<EditorElement*>& actors = EditorLayer::GetInstance()->GetEditorElements();
    for (size_t i = 0; i < mCurrentChunkData.ActorCount; ++i)
    {
        actors[i]->SaveProperties(ofs);
        actors[i]->SaveComponents(ofs);
    }
}

void ChunkLoader::LoadChunk(const std::string fileName)
{
    FileIOHelper::LoadFileIOData(GetConvertedFileName(fileName, CHUNK_FORMAT, FILE_IO_FORMAT));
    std::ifstream file;
    file.open(fileName, std::ios::binary);
    assert(file);
    if (file.is_open())
    {
        LoadChunkData(file);
        LoadActors(file);
        file.close();
    }
}

void ChunkLoader::LoadChunkToEditor(const std::string fileName)
{
    FileIOHelper::LoadFileIOData(GetConvertedFileName(fileName, CHUNK_FORMAT, FILE_IO_FORMAT));
    std::ifstream file;
    file.open(fileName, std::ios::binary);
    assert(file);
    if (file.is_open())
    {
        LoadChunkData(file);
        LoadActorsToEditor(file);
        file.close();
    }
}

void ChunkLoader::LoadChunkData(std::ifstream& ifs)
{
    /*
    ChunkData save order
        1) Camera2D values
            a) TargetActorID    -> int
            a) RenderResolution -> FTVector2
            b) ScreenCenter     -> FTVector2
        2) Number of Actors     -> int
    */
    mCurrentChunkData = {};
    mCurrentChunkData.TargetActorID = FileIOHelper::LoadInt(ifs);
    mCurrentChunkData.RenderResolution = FileIOHelper::LoadVector2(ifs);
    mCurrentChunkData.ScreenCenter = FileIOHelper::LoadVector2(ifs);
    mCurrentChunkData.ActorCount = FileIOHelper::LoadInt(ifs);

    Camera2D::GetInstance()->SetTargetActorID(mCurrentChunkData.TargetActorID);
    Camera2D::GetInstance()->SetRenderResolution(mCurrentChunkData.RenderResolution);
    Camera2D::GetInstance()->SetScreenCenter(mCurrentChunkData.ScreenCenter);
}

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
#endif // _DEBUG

ChunkLoader::ChunkLoader()
    : mCurrentChunkData{}
{
    mComponentLoadMap =
    {
        {L"AIComponent",          &Component::Load<AIComponent>},
        {L"AnimatorComponent",    &Component::Load<AnimatorComponent>},
        {L"AnimSpriteComponent",  &Component::Load<SpriteAnimComponent>},
        {L"BGSpriteComponent",    &Component::Load<BGSpriteComponent>},
        {L"ColliderComponent",    &Component::Load<ColliderComponent>},
        {L"InputMoveComponent",   &Component::Load<InputMoveComponent>},
        {L"MoveComponent",        &Component::Load<MoveComponent>},
        {L"Rigidbody2DComponent", &Component::Load<Rigidbody2DComponent>},
        {L"SpriteRendererComponent",      &Component::Load<SpriteRendererComponent>},
        {L"TileMapComponent",     &Component::Load<TileMapComponent>},
        {L"GunFiringComponent",   &Component::Load<GunFiringComponent>},
        {L"MeshRendererComponent", &Component::Load<MeshRendererComponent>},
        {L"SpineAnimRendererComponent", &Component::Load<SpineAnimRendererComponent>}
    };

    mComponentCreateMap =
    {
        {L"AIComponent",          &Component::Create<AIComponent>},
        {L"AnimatorComponent",    &Component::Create<AnimatorComponent>},
        {L"AnimSpriteComponent",  &Component::Create<SpriteAnimComponent>},
        {L"BGSpriteComponent",    &Component::Create<BGSpriteComponent>},
        {L"ColliderComponent",    &Component::Create<ColliderComponent>},
        {L"InputMoveComponent",   &Component::Create<InputMoveComponent>},
        {L"MoveComponent",        &Component::Create<MoveComponent>},
        {L"Rigidbody2DComponent", &Component::Create<Rigidbody2DComponent>},
        {L"SpriteRendererComponent", &Component::Create<SpriteRendererComponent>},
        {L"TileMapComponent",     &Component::Create<TileMapComponent>},
        {L"GunFiringComponent",   &Component::Create<GunFiringComponent>},
        {L"MeshRendererComponent", &Component::Create<MeshRendererComponent>},
        {L"SpineAnimRendererComponent", &Component::Create<SpineAnimRendererComponent>}
    };
};

ChunkLoader::~ChunkLoader(){}

void FileIOHelper::AddVector2(std::ofstream& ofs, FTVector2 value)
{
    AddFloat(ofs, value.x);
    AddFloat(ofs, value.y);
}

void FileIOHelper::AddVector3(std::ofstream& ofs, FTVector3 value)
{
    AddFloat(ofs, value.x);
    AddFloat(ofs, value.y);
    AddFloat(ofs, value.z);
}

void FileIOHelper::AddBasicString(std::ofstream& ofs, std::string value)
{
    //ofs.write((char*)&value[0], STRING_BUFFER_SIZE);
    ofs << value << std::endl;
    ++mUnmatched;
}

void FileIOHelper::AddWString(std::ofstream& ofs, std::wstring value)
{
    //ofs.write((char*)&value[0], WSTRING_BUFFER_SIZE);
    ofs << ToString(value) << std::endl;
    ++mUnmatched;
}

void FileIOHelper::AddFloat(std::ofstream& ofs, float value)
{
    //ofs.write((char*)&value, sizeof(float));
    ofs << value << std::endl;
    ++mUnmatched;
}

void FileIOHelper::AddInt(std::ofstream& ofs, int value)
{
    //ofs.write((char*)&value, sizeof(int));
    ofs << value << std::endl;
    ++mUnmatched;
}

void FileIOHelper::AddSize(std::ofstream& ofs, size_t value)
{
    //ofs.write((char*)&value, sizeof(size_t));
    ofs << value << std::endl;
    ++mUnmatched;
}

FTVector2 FileIOHelper::LoadVector2(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    float x = LoadFloat(ifs);
    float y = LoadFloat(ifs);
    return FTVector2(x, y);
}

FTVector3 FileIOHelper::LoadVector3(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    float x = LoadFloat(ifs);
    float y = LoadFloat(ifs);
    float z = LoadFloat(ifs);
    return FTVector3(x, y, z);
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