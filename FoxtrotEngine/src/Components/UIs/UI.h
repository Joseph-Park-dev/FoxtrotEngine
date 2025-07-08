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
class FTRectArea;

class UI :
	public Component
{
public:
	virtual FTDS::String GetName() const override
	{
		return "UI";
	}

public:
	bool		 IsMouseHovering();
	bool		 LeftButtonIsDown();
	bool		 LeftButtonIsClicked();
	bool		 GetIsFocused();
	bool		 GetIsAffectedByCamera();
	FTRectangle* GetDebugShape() const;

	void SetIsFocused(bool isFocused);
	void SetIsAffectedByCamera(bool affected);
	void SetMouseHovering(bool hovering);
	void SetDebugShape(FTRectangle* rect);
	// void SetColorID(uint8_t r, uint8_t g, uint8_t b);

	FTRectArea* RectArea();

public:
	virtual void CheckMouseHover(FTVector2 mousePos);
	virtual void OnMouseHovering();
	virtual void OnMouseLButtonDown();
	virtual void OnMouseLButtonUp();
	virtual void OnMouseLButtonClicked();

public:
	virtual void Initialize(FTCore* ftCoreInst) override;
	virtual void ProcessInput(FTInputDevice* inputDevice) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	UI(Actor* owner,
	   int	  updateOrder = DefaultVal::UPDATE_ORDER);
	virtual ~UI() override;

private:
	bool mIsAffectedByCamera;
	bool mMouseHovering;
	bool mLBtnDown;
	bool mLBtnClicked;
	bool mIsFocused;

	// Needed to get Render Width & Render Height
	FoxtrotRenderer* mRenderer;
	FTRectArea*		 mRectArea;
	FTRectangle*	 mDBGShape;

	// uint8_t			 mColorID[4];
	//  An area this UI can receive input from.

	friend class UIManager;

private:
	void UpdateDebugShape(Camera* camInst);

public:
	void SaveProperties(std::ofstream& ifs) override;
	void LoadProperties(std::ifstream& ofs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorRender(FoxtrotRenderer* renderer) override;
	virtual void EditorUIUpdate() override;
#endif
};

namespace ChunkKey
{
	constexpr const char* UI_SHOW_DEBUG_SHAPE = "Show Debug Shape";
}