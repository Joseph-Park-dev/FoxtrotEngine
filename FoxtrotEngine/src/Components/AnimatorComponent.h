#pragma once
#include "Components/TileMapComponent.h"

#include <unordered_map>
#include <string>

#include "Core/TemplateFunctions.h"

class FoxtrotRenderer;
class FTSpriteAnimation;

class AnimatorComponent :
    public TileMapComponent
{
public:
    void Play(bool isRepeated);  // Play current animation
    void Play(const UINT key, bool isRepeated);
    void Stop();
    void CreateAnimationFromTile(std::string&& name, UINT texKey, UINT tileMapKey);
    void LoadAnimation(const UINT key);
    //void SaveAnimation(const std::wstring& animName, const std::wstring& path);
    //void LoadAnimation(const std::wstring& path);
    
public:
    virtual std::string GetName() const override
    {
        return "AnimatorComponent";
    }
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
    virtual void Initialize(FTCore* coreInstance)   override;
            void Update(float deltaTime)            override;
            void LateUpdate(float deltaTime)        override;
    virtual void Render(FoxtrotRenderer* renderer)  override;

public:
    AnimatorComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
    ~AnimatorComponent() override;

private:
    std::vector<UINT>   mLoadedKeys;
    FTSpriteAnimation*  mCurrentAnim;

#ifdef FOXTROT_EDITOR
public:
    virtual void EditorUpdate(float deltaTime) override;

public:
    virtual void EditorUIUpdate() override;
            void UpdatePlayAnim();
            void UpdatePlayList();
            void CreateAnimation();

    virtual void OnConfirmUpdate() override;

public:
    virtual void CloneTo(Actor* actor) override;
#endif // FOXTROT_EDITOR
};