#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorCamera2D.h"
#include "EditorLayer.h"
#include "FTCore.h"
#include "KeyInputManager.h"

void EditorCamera2D::ProcessInput(MSG msg)
{
	int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
	if (msg.message == WM_MOUSEWHEEL);
	{
		if (delta > 0) // scroll up
			ZoomOut();
		else if (delta < 0) // scroll down
			ZoomIn();
	}
}

void EditorCamera2D::Update(float deltaTime)
{
	CalcDiff();
	MiddleMouseNavigation();
	CalcCameraDisplay();
	UpdateGrid();
}

void EditorCamera2D::MiddleMouseNavigation()
{
	FTVector2 diff = FTVector2::Zero;
	if (MOUSE_TAP(MOUSE::MOUSE_MIDDLE))
	{
		mMiddleMouseClickedPos = MOUSE_POS;
	}
	else if (MOUSE_HOLD(MOUSE::MOUSE_MIDDLE))
	{
		diff = (MOUSE_POS - mMiddleMouseClickedPos) * mMouseNavFactor;
		FTVector2 lookAtPos = FTVector2(this->GetLookAtPos().x - diff.x, this->GetLookAtPos().y + diff.y);
		this->SetLookAtPos(lookAtPos);
		//SetScreenCenter(GetScreenCenter() + diff);
		//Camera2D::GetInstance()->SetScreenCenter(GetScreenCenter() - diff);
		mMiddleMouseClickedPos = MOUSE_POS;
		KeyInputManager::GetInstance()->LockCursorInSceneViewport(MOUSE_POS);
	}
	else
		KeyInputManager::GetInstance()->UnlockCursorOutOfSceneViewport();
}

void EditorCamera2D::CalcCameraDisplay()
{
	Bounds* camRenderArea = Camera2D::GetInstance()->GetRenderArea();
	FTVector2 camLookAtPos = FTVector2(camRenderArea->x, camRenderArea->y);
	FTVector2 displayPos = EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(camLookAtPos);
	mCameraDisplay.x = displayPos.x;
	mCameraDisplay.y = displayPos.y;
	mCameraDisplay.w = camRenderArea->w * (1- GetZoomValue());
	mCameraDisplay.h = camRenderArea->h * (1 - GetZoomValue());
}

void EditorCamera2D::UpdateGrid()
{
	if (ImGui::IsKeyPressed(ImGuiKey_G))
		mGridIsOn = !mGridIsOn;
}

void EditorCamera2D::EditorRender(FoxtrotRenderer* renderer)
{
	/*
	Alternative for
		SDL_SetRenderDrawColor
		(
			renderer,
			255, 255, 255, 255
		);
		SDL_RenderDrawRect(renderer, &mCameraDisplay);
	
	*/

	//if(mGridIsOn)
	//	RenderGrid(renderer);
}
//
//void EditorCamera2D::RenderGrid(FoxtrotRenderer* renderer)
//{
//	mEditorWindowSize = FTCore::GetInstance()->GetEditorWindowSize();
//	int gridHCount = mEditorWindowSize.x / mGridCellSize;
//	int gridVCount = mEditorWindowSize.y / mGridCellSize;
//
//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
//	for (int x = 0; x < 1 + gridHCount * mGridCellSize;
//		x += mGridCellSize) {
//		SDL_RenderDrawLine(renderer, x, 0, x, mEditorWindowSize.y);
//	}
//
//	for (int y = 0; y < 1 + gridVCount * mGridCellSize;
//		y += mGridCellSize) {
//		SDL_RenderDrawLine(renderer, 0, y, mEditorWindowSize.x, y);
//	}
//}

void EditorCamera2D::DisplayCameraMenu()
{
	ImGui::Begin("Main Camera");
	// Set LookAt Pos
	FTVector2 lookAtPos = Camera2D::GetInstance()->GetLookAtPos();
	float* posArr = new float[2];
	posArr[0] = lookAtPos.x;
	posArr[1] = lookAtPos.y;
	ImGui::DragFloat2("Look-At Pos", posArr);
	FTVector2 updatedLookAtPos = FTVector2(posArr[0], posArr[1]);
	delete[] posArr;
	Camera2D::GetInstance()->SetLookAtPos(updatedLookAtPos);

	// Updating screen center since the camera position is moved
	FTVector2 diff = updatedLookAtPos - lookAtPos;
	FTVector2 screenCenter = Camera2D::GetInstance()->GetScreenCenter();
	Camera2D::GetInstance()->SetScreenCenter(screenCenter + diff);

	// Set Target 
	std::vector<EditorElement*>& editorElems = EditorLayer::GetInstance()->GetEditorElements();
	std::string* actorNames = new std::string[editorElems.size() + 1];
	actorNames[0] = "None";
	for (UINT i = 0; i < editorElems.size(); ++i)
	{
		actorNames[i + 1] = ToString(editorElems[i]->GetName());
	}
	if (editorElems.size() > 0)
	{
		const char* comboPreview = actorNames[mCurrIdx].c_str();
		if (ImGui::BeginCombo("Target Actor", comboPreview))
		{
			for (UINT i = 0; i < editorElems.size() + 1; ++i)
			{
				if (ImGui::Selectable(actorNames[i].c_str()))
				{
					mCurrIdx = i;
					if (mCurrIdx == 0)
						Camera2D::GetInstance()->SetTargetActorID(TARGET_NONE);
					else if (0 < mCurrIdx)
					{
						Camera2D::GetInstance()->SetTargetActorID(editorElems[mCurrIdx - 1]->GetID());
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	delete[] actorNames;
	ImGui::End();
}

EditorCamera2D::EditorCamera2D()
	: Camera2D()
	, mMiddleMouseClickedPos(FTVector2::Zero)
	, mCameraDisplay{}
	, mCurrIdx(0)
	, mGridCellSize(64.f)
	, mMouseNavFactor(0.001f)
{
	SetTargetActorID(TARGET_NONE);
	SetScreenCenter(Camera2D::GetInstance()->GetScreenCenter());

	//FTVector2 windowSize = FTCore::GetInstance()->GetEditorWindowSize();
	//int gridHCount = windowSize.x / mGridCellSize + 2;
	//int gridVCount = windowSize.y / mGridCellSize + 2;
}

EditorCamera2D::~EditorCamera2D() {}
#endif