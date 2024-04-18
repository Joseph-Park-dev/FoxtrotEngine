#ifdef _DEBUG
#pragma once
#include "SingletonMacro.h"
#include "Math.h"
#include "Camera2D.h"

class EditorCamera2D : public Camera2D
{
	SINGLETON(EditorCamera2D);

public:
	//void ProcessInput(SDL_Event* event);
	void Update(float deltaTime) override;
	void EditorRender(FoxtrotRenderer* renderer);
	
	void DisplayCameraMenu();

private:
	Vector2		mMiddleMouseClickedPos;
	Bounds		mCameraDisplay;
	Vector2		mDisplayPosition;

	Vector2		mEditorWindowSize;
	float		mGridCellSize;
	bool		mGridIsOn;

private:
	void MiddleMouseNavigation();
	void CalcCameraDisplay();
	void UpdateGrid();
	//void RenderGrid(FoxtrotRenderer* renderer);
	
private:
	UINT mCurrIdx;
};
#endif // _DEBUG