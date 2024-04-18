#pragma once
#include <unordered_map>
#include <string>

#include "Component.h"
#include "Animation.h"
#include "TileMapComponent.h"
#include "TemplateFunctions.h"

class FoxtrotRenderer;

class AnimatorComponent :
    public TileMapComponent
{
public:
    void CreateAnimationFromTile
    (
        const std::wstring& name, 
        const std::wstring& fileName, 
        int tileSizeX, 
        int tileSizeY, 
        float speed
    );
    void Play(const std::wstring& name, bool isRepeating);
    void SaveAnimation(const std::wstring& animName, const std::wstring& path);
    void LoadAnimation(const std::wstring& path);
    
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
    std::unordered_map<std::wstring, Animation*> mMapAnimation;
    Animation*          mCurrentAnim;
    bool                mIsRepeating;

private:
    Animation* FindAnimation(const std::wstring& name);

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

public:
    virtual void EditorLateUpdate(float deltaTime) override;
};