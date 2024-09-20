#pragma once
#include "FoxtrotEngine/Components/TileMapComponent.h"

#include <unordered_map>
#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class FoxtrotRenderer;
class FTSpriteAnimation;

class AnimatorComponent :
    public Component
{
public:
    void CreateAnimationFromTile
    (
        const std::string& name, 
        const std::string& fileName, 
        int tileSizeX, 
        int tileSizeY, 
        float speed
    );
    void Play(const std::string& name, bool isRepeating);
    //void SaveAnimation(const std::wstring& animName, const std::wstring& path);
    //void LoadAnimation(const std::wstring& path);
    
public:
    std::wstring GetName() const override
    {
        return L"AnimatorComponent";
    }

public:
    void LateUpdate(float deltaTime) override;
    virtual void Render(FoxtrotRenderer* renderer) override;

public:
    AnimatorComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
    ~AnimatorComponent();

private:
    std::unordered_map<std::wstring, FTSpriteAnimation*> mMapAnimation;
    FTSpriteAnimation*  mCurrentAnim;
    bool                mIsRepeating;

private:
    FTSpriteAnimation* FindAnimation(const std::wstring& name);

public:
    virtual void SaveProperties(nlohmann::ordered_json& out) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

public:
    virtual void EditorLateUpdate(float deltaTime) override;
};