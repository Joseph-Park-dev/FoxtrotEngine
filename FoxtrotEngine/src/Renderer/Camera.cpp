#include "Renderer/Camera.h"

#include "directxtk/SimpleMath.h"

#include "Core/FTCore.h"
#include "Managers/KeyInputManager.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "EditorLayer.h"
#endif // FOXTROT_EDITOR

void Camera::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;
}

void Camera::Update(float deltaTime)
{
	CalcNDCRatio();
}

void Camera::ZoomIn()
{
}

Camera::Camera()
{}

Camera::~Camera(){}

void Camera::CalcNDCRatio()
{
	mNDCRatio =
		static_cast<float>(2) / static_cast<float>(mRenderer->GetRenderHeight());
}

Matrix Camera::GetViewRow()
{
	return	Matrix::CreateTranslation(-mPosition) *
			Matrix::CreateRotationY(-mYaw) *
			Matrix::CreateRotationX(mPitch);
}

Matrix Camera::GetProjRow()
{
	float unitsPerPixel = 1.f/35.5f; // 1 unit = 100 pixels
	float worldWidth = mRenderer->GetRenderWidth() * unitsPerPixel;
	float worldHeight = mRenderer->GetRenderHeight() * unitsPerPixel;

	return mViewType == Viewtype::Perspective
		? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mProjFOVAngleY),
			mAspect, mNearZ, mFarZ)
		: DirectX::XMMatrixOrthographicOffCenterLH(
			0.0f, worldWidth, 
			worldHeight, 0.0f, 
			mNearZ, mFarZ
		);
}

Vector3 Camera::GetEyePos()
{
	return mPosition;
}

#ifdef FOXTROT_EDITOR
void Camera::DisplayCameraMenu()
{
	ImGui::Begin("Main Camera");
	// Set LookAt Pos

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
	{
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle) * Drag_MODSPEED;
		if (mMiddleMouseClickedPos != delta)
		{
			mMiddleMouseClickedPos = delta;
			// mPosition += FTVector3(-delta.x, delta.y, 0.f);
		}
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left) * 0.0001;
		if (mMiddleMouseClickedRot != delta)
		{
			mMiddleMouseClickedRot = delta;
			// mViewEyeRotation -= FTVector3(delta.y, delta.x, 0.f);
		}
	}
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", mPosition, LOOKAT_MODSPEED);
	CommandHistory::GetInstance()->UpdateFloatValue("Look-At Yaw", &mYaw, LOOKAT_MODSPEED);
	CommandHistory::GetInstance()->UpdateFloatValue("Look-At Pitch", &mPitch, LOOKAT_MODSPEED);

	//// Updating screen center since the camera position is moved
	// FTVector2 diff = updatedLookAtPos - lookAtPos;
	// FTVector2 screenCenter = Camera2D::GetInstance()->GetScreenCenter();
	// Camera2D::GetInstance()->SetScreenCenter(screenCenter + diff);

	//// Set Target
	// std::vector<EditorElement*>& editorElems = EditorLayer::GetInstance()->GetEditorElements();
	// std::string* actorNames = new std::string[editorElems.size() + 1];
	// actorNames[0] = "None";
	// for (UINT i = 0; i < editorElems.size(); ++i)
	//{
	//	actorNames[i + 1] = ToString(editorElems[i]->GetName());
	// }
	// if (editorElems.size() > 0)
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
	// }
	// delete[] actorNames;
	ImGui::End();
}
#endif // FOXTROT_EDITOR