#pragma once
#include "Components/TileMapComponent.h"

#include <unordered_map>
#include <string>

#include "Core/TemplateFunctions.h"

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
    virtual std::wstring GetName() const override
    {
        return L"AnimatorComponent";
    }
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
    virtual void Initialize(FTCore* coreInstance) override {};
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

#ifdef FOXTROT_EDITOR
public:
    virtual void SaveProperties(nlohmann::ordered_json& out) override;
#endif
};