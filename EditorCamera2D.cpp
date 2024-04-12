#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include "EditorCamera2D.h"
#include "EditorLayer.h"
#include "CCore.h"
#include "KeyInputManager.h"

void EditorCamera2D::ProcessInput(SDL_Event* event)
{
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow))
	{
		if (event->type == SDL_MOUSEWHEEL)
		{
			if (event->wheel.y > 0) // scroll up
				ZoomOut();
			else if (event->wheel.y < 0) // scroll down
				ZoomIn();
		}
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
	Vector2 diff = Vector2::Zero;
	if (MOUSE_TAP(MOUSE::MOUSE_MIDDLE))
	{
		mMiddleMouseClickedPos = MOUSE_POS;
	}
	else if (MOUSE_HOLD(MOUSE::MOUSE_MIDDLE))
	{
		diff = (MOUSE_POS - mMiddleMouseClickedPos);
		this->SetLookAtPos(this->GetLookAtPos() - diff);
		//SetScreenCenter(GetScreenCenter() + diff);
		//Camera2D::GetInstance()->SetScreenCenter(GetScreenCenter() - diff);
		mMiddleMouseClickedPos = MOUSE_POS;
	}
}

void EditorCamera2D::CalcCameraDisplay()
{
	SDL_Rect* camRenderArea = Camera2D::GetInstance()->GetRenderArea();
	Vector2 camLookAtPos = Vector2(camRenderArea->x, camRenderArea->y);
	Vector2 displayPos = EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(camLookAtPos);
	mCameraDisplay.x = displayPos.x;
	mCameraDisplay.y = displayPos.y;
	mCameraDisplay.w = camRenderArea->w * (1- GetZoomValue());
	mCameraDisplay.h = camRenderArea->h * (1 - GetZoomValue());
}

void EditorCamera2D::UpdateGrid()
{
	if (ImGui::IsKeyPressed(ImGuiKey_G))
	{
		mGridIsOn = !mGridIsOn;
	}
}

void EditorCamera2D::EditorRender(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor
	(
		renderer,
		255, 255, 255, 255
	);
	SDL_RenderDrawRect(renderer, &mCameraDisplay);

	if(mGridIsOn)
		RenderGrid(renderer);
}

void EditorCamera2D::RenderGrid(SDL_Renderer* renderer)
{
	mEditorWindowSize = CCore::GetInstance()->GetEditorWindowSize();
	int gridHCount = mEditorWindowSize.x / mGridCellSize;
	int gridVCount = mEditorWindowSize.y / mGridCellSize;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
	for (int x = 0; x < 1 + gridHCount * mGridCellSize;
		x += mGridCellSize) {
		SDL_RenderDrawLine(renderer, x, 0, x, mEditorWindowSize.y);
	}

	for (int y = 0; y < 1 + gridVCount * mGridCellSize;
		y += mGridCellSize) {
		SDL_RenderDrawLine(renderer, 0, y, mEditorWindowSize.x, y);
	}
}

void EditorCamera2D::DisplayCameraMenu()
{
	if (ImGui::Begin("Main Camera"))
	{
		// Set LookAt Pos
		Vector2 lookAtPos = Camera2D::GetInstance()->GetLookAtPos();
		float* posArr = new float[2];
		posArr[0] = lookAtPos.x;
		posArr[1] = lookAtPos.y;
		ImGui::DragFloat2("Look-At Pos", posArr);
		Vector2 updatedLookAtPos = Vector2(posArr[0], posArr[1]);
		delete[] posArr;
		Camera2D::GetInstance()->SetLookAtPos(updatedLookAtPos);

		// Updating screen center since the camera position is moved
		Vector2 diff = updatedLookAtPos - lookAtPos;
		Vector2 screenCenter = Camera2D::GetInstance()->GetScreenCenter();
		Camera2D::GetInstance()->SetScreenCenter(screenCenter + diff);

		// Set Target 
		std::vector<EditorElement*>& editorElems = EditorLayer::GetInstance()->GetEditorElements();
		std::string* actorNames = new std::string[editorElems.size()+1];
		actorNames[0] = "None";
		for (Uint32 i = 0; i < editorElems.size(); ++i)
		{
			actorNames[i+1] = ToString(editorElems[i]->GetName());
		}
		if (editorElems.size() > 0)
		{
			const char* comboPreview = actorNames[mCurrIdx].c_str();
			if (ImGui::BeginCombo("Target Actor", comboPreview))
			{
				for (Uint32 i = 0; i < editorElems.size()+1; ++i)
				{
					if (ImGui::Selectable(actorNames[i].c_str()))
					{
						mCurrIdx = i;
						if (mCurrIdx == 0)
							Camera2D::GetInstance()->SetTargetActorID(TARGET_NONE);
						else if (0 < mCurrIdx)
						{
							Camera2D::GetInstance()->SetTargetActorID(editorElems[mCurrIdx-1]->GetID());
						}
					}
				}
				ImGui::EndCombo();
			}
		}
		delete[] actorNames;
		ImGui::End();
	}
}

EditorCamera2D::EditorCamera2D()
	: Camera2D()
	, mMiddleMouseClickedPos(Vector2::Zero)
	, mCameraDisplay{}
	, mCurrIdx(0)
	, mGridCellSize(64.f)
{
	SetTargetActorID(TARGET_NONE);
	SetScreenCenter(Camera2D::GetInstance()->GetScreenCenter());

	Vector2 windowSize = CCore::GetInstance()->GetEditorWindowSize();
	int gridHCount = windowSize.x / mGridCellSize + 2;
	int gridVCount = windowSize.y / mGridCellSize + 2;
}

EditorCamera2D::~EditorCamera2D() {}
#endif