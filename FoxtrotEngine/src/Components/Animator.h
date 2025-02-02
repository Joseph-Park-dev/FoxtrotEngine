// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Takes care of all FTAnimations registered.
/// All types of animation are handled here as FTResource derived from
/// the same class, FTAnimation.
/// (sprite animation, spine animation, etc.)
/// </summary>

#pragma once
#include "Components/TileMapRenderer.h"

#include <unordered_map>
#include <string>

#include "Core/TemplateFunctions.h"

class FoxtrotRenderer;
class FTSpriteAnimation;

class Animator :
    public TileMapRenderer
{
public:
    virtual std::string GetName() const override
    {
        return "Animator";
    }

public:
    void Play(bool isRepeated);  // Play current animation
    void Play(const UINT key, bool isRepeated);
    void Stop();

public:
    FTSpriteAnimation* CreateAnimationFromTile(
        std::string&& name, 
        UINT texKey, 
        UINT tileMapKey
    );
    void LoadAnimation(const UINT key);
    
public:
    virtual void Initialize (FTCore* coreInstance)      override;
            void Update     (float deltaTime)           override;
            void LateUpdate (float deltaTime)           override;
    virtual void Render     (FoxtrotRenderer* renderer) override;

    virtual void CloneTo    (Actor* actor)              override;

public:
     Animator(
         class Actor* owner, 
          
         int updateOrder = DEFAULT_UPDATEORDER
     );
    ~Animator() override;

private:
    std::vector<UINT>               mLoadedKeys;
    std::vector<FTSpriteAnimation*> mLoadedAnimations;
    FTSpriteAnimation*              mCurrentAnim;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
    virtual void EditorUpdate   (float deltaTime)   override;
    virtual void EditorUIUpdate ()                  override;

private:
            void UpdatePlayAnim();
            void UpdatePlayList();
            void CreateAnimation();
#endif // FOXTROT_EDITOR
};