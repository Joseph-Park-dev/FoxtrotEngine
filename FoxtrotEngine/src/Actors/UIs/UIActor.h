#pragma once
#include "Actors/Actor.h"

#include "Core/TemplateFunctions.h"
#include "Actors/RectTransform.h"

class FoxtrotRenderer;

class UIActor :
    public Actor
{
public:
    bool IsMouseHovering() { return mMouseHovering; }
    bool LeftButtonIsDown() { return mLBtnDown; }
    bool LeftButtonIsClicked() { return mLBtnClicked; }
    bool GetIsFocused() { return mIsFocused; }
    void SetIsFocused(bool isFocused) { mIsFocused = isFocused; }

public:
    virtual void CheckMouseHover();
    virtual void OnMouseHovering();
    virtual void OnMouseLButtonDown();
    virtual void OnMouseLButtonUp();
    virtual void OnMouseLButtonClicked();

public:
    RectTransform* GetRectTransform() { return dynamic_cast<RectTransform*>(GetTransform()); }
    bool GetIsAffectedByCamera() { return mIsAffectedByCamera; }
    void SetIsAffectedByCamera(bool affected) { mIsAffectedByCamera = affected; }

    void SetMouseHovering(bool hovering) { mMouseHovering = hovering; }

public:
    virtual void UpdateActor    (float deltaTime) override;
    virtual void LateUpdateActor(float deltaTime) override;
    virtual void RenderActor    (FoxtrotRenderer* renderer) override;

private:
    bool mIsAffectedByCamera;
    bool mMouseHovering;
    bool mLBtnDown;
    bool mLBtnClicked;

    bool mIsFocused;

    friend class UIManager;

public:
    UIActor(Scene* scene, bool isCamAffect);
    UIActor(UIActor& origin, Scene* scene);
    UIActor(Actor* origin, Scene* scene);
    virtual ~UIActor() override;
};

