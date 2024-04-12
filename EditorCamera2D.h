#ifdef _DEBUG
#pragma once
#include "SingletonMacro.h"
#include "Math.h"
#include "Camera2D.h"
#include <SDL2/SDL_render.h>

class EditorCamera2D : public Camera2D
{
	SINGLETON(EditorCamera2D);

public:
	void ProcessInput(SDL_Event* event);
	void Update(float deltaTime) override;
	void EditorRender(SDL_Renderer* renderer);
	
	void DisplayCameraMenu();

private:
	Vector2		mMiddleMouseClickedPos;
	SDL_Rect	mCameraDisplay;
	Vector2		mDisplayPosition;

	Vector2		mEditorWindowSize;
	float		mGridCellSize;
	bool		mGridIsOn;

private:
	void MiddleMouseNavigation();
	void CalcCameraDisplay();
	void UpdateGrid();
	void RenderGrid(SDL_Renderer* renderer);
	
private:
	Uint32 mCurrIdx;
};
#endif // _DEBUG