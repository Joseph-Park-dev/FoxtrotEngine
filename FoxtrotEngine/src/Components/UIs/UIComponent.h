#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"
#include "Actors/RectTransform.h"

class FoxtrotRenderer;
class FTRectangle;

class UIComponent :
    public Component
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
    //RectTransform* GetRectTransform() { return dynamic_cast<RectTransform*>(GetTransform()); }
    bool GetIsAffectedByCamera() { return mIsAffectedByCamera; }
    void SetIsAffectedByCamera(bool affected) { mIsAffectedByCamera = affected; }

    void SetMouseHovering(bool hovering) { mMouseHovering = hovering; }

public:
    virtual void Initialize(FTCore* ftCoreInst) override;
    virtual void Update(float deltaTime);
    virtual void LateUpdate(float deltaTime);
    virtual void Render(FoxtrotRenderer* renderer);


public:
    UIComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
    virtual ~UIComponent() override;

private:
    bool mIsAffectedByCamera;
    bool mMouseHovering;
    bool mLBtnDown;
    bool mLBtnClicked;
    bool mIsFocused;
    FTVector2 mSize;

    friend class UIManager;

#ifdef FOXTROT_EDITOR
public:
    virtual void EditorUpdate(float deltaTime) override;
    virtual void EditorRender(FoxtrotRenderer* renderer) override;
    virtual void EditorUIUpdate() override;

private:
    FTRectangle* mDebugRect;
#endif // FOXTROT_EDITOR
};