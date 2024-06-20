#ifdef _DEBUG
#pragma once
#include "SingletonMacro.h"
#include "FTMath.h"
#include "Camera2D.h"

class EditorCamera2D : public Camera2D
{
	SINGLETON(EditorCamera2D);

public:
	void ProcessInput(MSG msg);
	void Update(float deltaTime) override;
	void EditorRender(FoxtrotRenderer* renderer);
	void DisplayCameraMenu();
	FTVector2 ConvertWorldPosToScreen(FTVector2 worldPos) const override;

public:
	float GetMouseNavFactor() const { return mMouseNavFactor; }

private:
	FTVector2	mMiddleMouseClickedPos;
	Bounds		mCameraDisplay;
	FTVector2	mDisplayPosition;
	// 0 ~ 1 value (This is used to reduce drastic change in lookAtPos)
	float		mMouseNavFactor;

	FTVector2	mEditorWindowSize;
	float		mGridCellSize;
	bool		mGridIsOn;

private:
	void MiddleMouseNavigation();
	void CalcCameraDisplay();
	void UpdateGrid();
	void CalcDiff() override;
	//void RenderGrid(FoxtrotRenderer* renderer);
	
private:
	UINT mCurrIdx;
};
#endif // _DEBUG