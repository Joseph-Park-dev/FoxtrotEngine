#include <string>
#include <fstream>
#include <Windows.h>
#include <commdlg.h>
#include <iostream>

#include "ChunkLoader.h"
#include "Math.h"
#include "TemplateFunctions.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ActorGroup.h"
#include "Transform.h"
#include "Actor.h"
#include "Ship.h"
#include "Asteroid.h"
#include "BackgroundActor.h"
#include "GroundObject.h"
#include "EditorLayer.h"
#include "PanelUI.h"
#include "Camera2D.h"
#include "EditorCamera2D.h"

#include "Component.h"
#include "AIComponent.h"
#include "AnimatorComponent.h"
#include "AnimSpriteComponent.h"
#include "BGSpriteComponent.h"
#include "ColliderComponent.h"
#include "InputMoveComponent.h"
#include "MoveComponent.h"
#include "Rigidbody2DComponent.h"
#include "SpriteComponent.h"
#include "TileMapComponent.h"
#include "GunFiringComponent.h"

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
            a) RenderResolution -> Vector2
            b) ScreenCenter     -> Vector2
        2) Number of Actors     -> int
    */
    // Saves Camera2D Values.
    FileIOHelper::AddInt(ofs, Camera2D::GetInstance()->GetTargetActorID());
    FileIOHelper::
        AddVector2(
            ofs, Camera2D::GetInstance()->GetRenderResolution()
        );
    FileIOHelper::
        AddVector2(
            ofs, Camera2D::GetInstance()->GetScreenCenter()
        );
    // Saves number of actors in the scene. 
    mCurrentChunk.ActorCount = EditorLayer::GetInstance()->GetEditorElements().size();
    FileIOHelper::AddInt(ofs, mCurrentChunk.ActorCount);
}

void ChunkLoader::SaveActors(std::ofstream& ofs)
{
    Actor::ResetNextID();
    const std::vector<EditorElement*>& actors = EditorLayer::GetInstance()->GetEditorElements();
    for (size_t i = 0; i < mCurrentChunk.ActorCount; ++i)
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
            a) RenderResolution -> Vector2
            b) ScreenCenter     -> Vector2
        2) Number of Actors     -> int
    */
    Camera2D::GetInstance()->SetTargetActorID(FileIOHelper::LoadInt(ifs));
    Camera2D::GetInstance()->SetRenderResolution(FileIOHelper::LoadVector2(ifs));
    Camera2D::GetInstance()->SetScreenCenter(FileIOHelper::LoadVector2(ifs));
    mCurrentChunk.ActorCount = FileIOHelper::LoadInt(ifs);
}

void ChunkLoader::LoadActors(std::ifstream& ifs)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
    size_t actorCount = mCurrentChunk.ActorCount;
    for (size_t i = 0; i < actorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(ifs, currScene);
        currScene->AddActor(actor, actor->GetActorGroup());
    }
}

void ChunkLoader::LoadActorsToEditor(std::ifstream& ifs)
{
    Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
    size_t actorCount = mCurrentChunk.ActorCount;
    for (size_t i = 0; i < actorCount; ++i)
    {
        Actor* actor = LoadIndividualActor(ifs, currScene);
        EditorLayer::GetInstance()->AddEditorElement(actor);
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
    : mCurrentChunk{}
{
    mComponentLoadMap =
    {
        {L"AIComponent",          &Component::Load<AIComponent>},
        {L"AnimatorComponent",    &Component::Load<AnimatorComponent>},
        {L"AnimSpriteComponent",  &Component::Load<AnimSpriteComponent>},
        {L"BGSpriteComponent",    &Component::Load<BGSpriteComponent>},
        {L"ColliderComponent",    &Component::Load<ColliderComponent>},
        {L"InputMoveComponent",   &Component::Load<InputMoveComponent>},
        {L"MoveComponent",        &Component::Load<MoveComponent>},
        {L"Rigidbody2DComponent", &Component::Load<Rigidbody2DComponent>},
        {L"SpriteComponent",      &Component::Load<SpriteComponent>},
        {L"TileMapComponent",     &Component::Load<TileMapComponent>},
        {L"GunFiringComponent",   &Component::Load<GunFiringComponent>}
    };

    mComponentCreateMap =
    {
        {L"AIComponent",          &Component::Create<AIComponent>},
        {L"AnimatorComponent",    &Component::Create<AnimatorComponent>},
        {L"AnimSpriteComponent",  &Component::Create<AnimSpriteComponent>},
        {L"BGSpriteComponent",    &Component::Create<BGSpriteComponent>},
        {L"ColliderComponent",    &Component::Create<ColliderComponent>},
        {L"InputMoveComponent",   &Component::Create<InputMoveComponent>},
        {L"MoveComponent",        &Component::Create<MoveComponent>},
        {L"Rigidbody2DComponent", &Component::Create<Rigidbody2DComponent>},
        {L"SpriteComponent",      &Component::Create<SpriteComponent>},
        {L"TileMapComponent",     &Component::Create<TileMapComponent>},
        {L"GunFiringComponent",   &Component::Create<GunFiringComponent>}
    };
};

ChunkLoader::~ChunkLoader(){}

void FileIOHelper::AddVector2(std::ofstream& ofs, Vector2 value)
{
    AddFloat(ofs, value.x);
    AddFloat(ofs, value.y);
}

void FileIOHelper::AddBasicString(std::ofstream& ofs, std::string value)
{
    ofs.write((char*)&value[0], STRING_BUFFER_SIZE);
    ++mUnmatched;
}

void FileIOHelper::AddWString(std::ofstream& ofs, std::wstring value)
{
    ofs.write((char*)&value[0], WSTRING_BUFFER_SIZE);
    ++mUnmatched;
}

void FileIOHelper::AddFloat(std::ofstream& ofs, float value)
{
    ofs.write((char*)&value, sizeof(float));
    ++mUnmatched;
}

void FileIOHelper::AddInt(std::ofstream& ofs, int value)
{
    ofs.write((char*)&value, sizeof(int));
    ++mUnmatched;
}

void FileIOHelper::AddSize(std::ofstream& ofs, size_t value)
{
    ofs.write((char*)&value, sizeof(size_t));
    ++mUnmatched;
}

Vector2 FileIOHelper::LoadVector2(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    float x = LoadFloat(ifs);
    float y = LoadFloat(ifs);

    return Vector2(x, y);
}

std::string FileIOHelper::LoadBasicString(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    char buffer[STRING_BUFFER_SIZE];
    ifs.read((char*)&buffer[0], STRING_BUFFER_SIZE);
    std::string str = std::string(buffer);
    return str;
}

std::wstring FileIOHelper::LoadWString(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    wchar_t buffer[WSTRING_BUFFER_SIZE];
    ifs.read((char*)&buffer[0], WSTRING_BUFFER_SIZE);
    std::wstring str = std::wstring(buffer);
    return str;
}

float FileIOHelper::LoadFloat(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    float val = 0.f;
    ifs.read((char*)&val, sizeof(float));

    return val;
}

int FileIOHelper::LoadInt(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    int val = 0;
    ifs.read((char*)&val, sizeof(int));
    return val;
}

size_t FileIOHelper::LoadSize(std::ifstream& ifs)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    size_t val = 0;
    ifs.read((char*)&val, sizeof(size_t));
    return val;
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, Vector2& value)
{
    assert(mUnmatched > 0);
    LoadFloat(ifs, value.x);
    LoadFloat(ifs, value.y);
}

void FileIOHelper::LoadBasicString(std::ifstream& ifs, std::string& value)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    char buffer[STRING_BUFFER_SIZE];
    ifs.read((char*)&buffer[0], STRING_BUFFER_SIZE);
    std::string str = std::string(buffer);
}

void FileIOHelper::LoadWString(std::ifstream& ifs, std::wstring& value)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    wchar_t buffer[WSTRING_BUFFER_SIZE];
    ifs.read((char*)&buffer[0], WSTRING_BUFFER_SIZE);
    value = std::wstring(buffer);
}

void FileIOHelper::LoadFloat(std::ifstream& ifs, float& value)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    ifs.read((char*)&value, sizeof(float));
}

void FileIOHelper::LoadInt(std::ifstream& ifs, int& value)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    ifs.read((char*)&value, sizeof(int));
}

void FileIOHelper::LoadSize(std::ifstream& ifs, size_t& value)
{
    assert(mUnmatched > 0);
    --mUnmatched;
    ifs.read((char*)&value, sizeof(size_t));
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

void SaveWString(const std::wstring& str, FILE* file)
{
    // Data serialization (데이터 직렬화)
    const wchar_t* name = str.c_str();
    size_t len = str.length();

    fwrite(&len, sizeof(size_t), 1, file);
    fwrite(name, sizeof(wchar_t), len, file);
    SDL_Log("Saved : %ls", name);
}

void LoadWString(std::wstring& str, FILE* file)
{
    size_t len = 0;
    fread(&len, sizeof(size_t), 1, file);
    wchar_t szBuff[256] = {};
    fread(szBuff, sizeof(wchar_t), len, file);
    str = szBuff;
    SDL_Log("Loaded : %ls", str.c_str());
}

void LoadDataFromFile(char* dst, FILE* file)
{
    int i = 0;
    while (true)
    {
        char c = (char)getc(file);
        if (c == '\n')
        {
            dst[i++] = '\0';
            break;
        }

        dst[i++] = c;
    }
}