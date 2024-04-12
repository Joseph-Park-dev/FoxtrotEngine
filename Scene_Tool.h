#pragma once
#include "Scene.h"
class Scene_Tool :
    public Scene
{
private:

public:
    void Enter() override;
    void Exit()  override;

    void LoadData() override;

public:
    void Update(float deltaTime) override;

public:
    Scene_Tool();
    ~Scene_Tool();
    //void SaveSceneData();
    /*void SaveScene(const std::wstring& path);
    void LoadSceneData();
    void LoadScene(const std::wstring& path);*/
};

