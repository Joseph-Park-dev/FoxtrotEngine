// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for UI Components. 
/// This detects the mouse inputs, such as hovering, clicking, etc.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class FoxtrotRenderer;
class FTRectangle;

class UIComponent :
    public Component
{
public:
    bool IsMouseHovering        ();
    bool LeftButtonIsDown       ();
    bool LeftButtonIsClicked    ();
    bool GetIsFocused           ();
    bool GetIsAffectedByCamera  ();

    void SetIsFocused           (bool isFocused);
    void SetIsAffectedByCamera  (bool affected);
    void SetMouseHovering       (bool hovering);

public:
    virtual void CheckMouseHover        ();
    virtual void OnMouseHovering        ();
    virtual void OnMouseLButtonDown     ();
    virtual void OnMouseLButtonUp       ();
    virtual void OnMouseLButtonClicked  ();

public:
    virtual void Initialize (FTCore* ftCoreInst)        override;
    virtual void Update     (float deltaTime)           override;
    virtual void LateUpdate (float deltaTime)           override;
    virtual void Render     (FoxtrotRenderer* renderer) override;

public:
             UIComponent(
                 Actor* owner, 
                 int drawOrder = DEFAULT_DRAWORDER, 
                 int updateOrder = DEFAULT_UPDATEORDER
             );
    virtual ~UIComponent() override;

private:
    bool        mIsAffectedByCamera;
    bool        mMouseHovering;
    bool        mLBtnDown;
    bool        mLBtnClicked;
    bool        mIsFocused;
    FTVector2   mSize;

    friend class UIManager;

#ifdef FOXTROT_EDITOR
public:
    virtual void EditorUpdate   (float deltaTime)           override;
    virtual void EditorRender   (FoxtrotRenderer* renderer) override;
    virtual void EditorUIUpdate ()                          override;

private:
    FTRectangle* mDebugRect;
#endif // FOXTROT_EDITOR
};