#include "FoxtrotEngine/Renderer/Camera.h"

#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEditor/CommandHistory.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"

void Camera::Initialize()
{
}

void Camera::Update(float deltaTime)
{
}

void Camera::ZoomIn()
{
}

void Camera::DisplayCameraMenu()
{
	ImGui::Begin("Main Camera");
	// Set LookAt Pos

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle) * Drag_MODSPEED;
		if (mMiddleMouseClickedPos != delta) {
			mMiddleMouseClickedPos = delta;
			mViewEyePosition += FTVector3(-delta.x, delta.y, 0.f);
		}
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left) * 0.0001;
		if (mMiddleMouseClickedRot != delta) {
			mMiddleMouseClickedRot = delta;
			mViewEyeRotation -= FTVector3(delta.y, delta.x, 0.f);
		}
	}
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", mViewEyePosition, LOOKAT_MODSPEED);
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Rotation", mViewEyeRotation, LOOKAT_MODSPEED);

	//// Updating screen center since the camera position is moved
	//FTVector2 diff = updatedLookAtPos - lookAtPos;
	//FTVector2 screenCenter = Camera2D::GetInstance()->GetScreenCenter();
	//Camera2D::GetInstance()->SetScreenCenter(screenCenter + diff);

	//// Set Target 
	//std::vector<EditorElement*>& editorElems = EditorLayer::GetInstance()->GetEditorElements();
	//std::string* actorNames = new std::string[editorElems.size() + 1];
	//actorNames[0] = "None";
	//for (UINT i = 0; i < editorElems.size(); ++i)
	//{
	//	actorNames[i + 1] = ToString(editorElems[i]->GetName());
	//}
	//if (editorElems.size() > 0)
	//{
	//	const char* comboPreview = actorNames[mCurrIdx].c_str();
	//	if (ImGui::BeginCombo("Target Actor", comboPreview))
	//	{
	//		for (UINT i = 0; i < editorElems.size() + 1; ++i)
	//		{
	//			if (ImGui::Selectable(actorNames[i].c_str()))
	//			{
	//				mCurrIdx = i;
	//				if (mCurrIdx == 0)
	//					Camera2D::GetInstance()->SetTargetActorID(TARGET_NONE);
	//				else if (0 < mCurrIdx)
	//				{
	//					Camera2D::GetInstance()->SetTargetActorID(editorElems[mCurrIdx - 1]->GetID());
	//				}
	//			}
	//		}
	//		ImGui::EndCombo();
	//	}
	//}
	//delete[] actorNames;
	ImGui::End();
}

Camera::Camera()
	: mViewEyePos	 (0.0f, 0.0f, -10.0f)
	, mViewEyeDir	 (0.0f, 0.0f, 1.0f)
	, mViewUp		 (0.0f, 1.0f, 0.0f)
	, mProjFOVAngleY (70.0f)
	, mNearZ		 (0.01f)
	, mFarZ			 (100.0f)
	, mRenderResolution(HD_RESOLUTION)
	, mAspect		 (GetAspectRatio())
	, mTargetActor	 (nullptr)
	, mTargetActorID (CAMERA_TARGET_NONE)
{}

Camera::~Camera(){}
