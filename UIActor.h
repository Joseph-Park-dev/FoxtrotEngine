#pragma once
#include "Actor.h"
#include "TemplateFunctions.h"

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
    bool GetIsAffectedByCamera() { return mIsAffectedByCamera; }
    void SetIsAffectedByCamera(bool affected) { mIsAffectedByCamera = affected; }

    void SetMouseHovering(bool hovering) { mMouseHovering = hovering; }

public:
    virtual void UpdateActor    (float deltaTime) override;
    virtual void LateUpdateActor(float deltaTime) override;
    virtual void RenderActor    (FoxtrotRenderer* renderer) override;

public:
    UIActor(Scene* scene, bool isCamAffect);
    UIActor(UIActor& origin);
    UIActor(Actor* origin);
    virtual ~UIActor() override;

private:
    bool mIsAffectedByCamera;
    bool mMouseHovering;
    bool mLBtnDown;
    bool mLBtnClicked;

    bool mIsFocused;

    friend class UIManager;
};

