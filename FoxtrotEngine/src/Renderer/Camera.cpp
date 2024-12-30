#include "Renderer/Camera.h"

#include "directxtk/SimpleMath.h"

#include "Core/FTCore.h"
#include "Managers/KeyInputManager.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "EditorLayer.h"
#endif // FOXTROT_EDITOR

void Camera::Initialize(FoxtrotRenderer* renderer, UINT pixels, float unit = 1)
{
	mRenderer = renderer;
	InitializePixelsPerUnit(pixels, unit);
}

void Camera::Update(float deltaTime)
{
}

void Camera::ZoomIn()
{
}

Camera::Camera()
	: mRenderer(nullptr)
	, mPosition(Vector3(0.0f, 0.0f, 0.0f))
	, mViewDir(Vector3(0.0f, 0.0f, 1.0f))
	, mUpDir(Vector3(0.0f, -1.0f, 0.0f))
	, mRightDir(Vector3(1.0f, 0.0f, 0.0f))
	, mPitch(0.0f)
	, mYaw(0.0f)
	, mProjFOVAngleY(70.f)
	, mNearZ(0.01f)
	, mFarZ(100.0f)
	, mAspect(1920.f / 1080.f)
	, mScreenCenter(FTVector2(1920.f, 1080.f) * 0.5f)
	, mPixelsPerUnit(0.f)
	, mViewType(Viewtype::Orthographic)
{}

Camera::~Camera(){}

Matrix Camera::GetViewRow()
{
	return	Matrix::CreateTranslation(-mPosition) *
			Matrix::CreateRotationY(-mYaw) *
			Matrix::CreateRotationX(mPitch);
}

Matrix Camera::GetProjRow()
{
	float unitsPerPixel = 1 / mPixelsPerUnit;
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

Viewtype Camera::GetViewType()
{
	return mViewType;
}

float Camera::GetProjFOVAngleY()
{
	return mProjFOVAngleY;
}

float Camera::GetAspectRatio()
{
	return mAspect;
}

float Camera::GetPixelsPerUnit()
{
	return mPixelsPerUnit;
}

void Camera::SetViewType(Viewtype viewType)
{
	mViewType = viewType;
}

void Camera::InitializePixelsPerUnit(float pixels, float units)
{
	mPixelsPerUnit = pixels / units;
}

FTVector3 Camera::ConvertScreenPosToWorld(FTVector2 screenPos)
{
	float screenWidth = mRenderer->GetRenderWidth();
	float screenHeight = mRenderer->GetRenderHeight();

	FTVector2 topLeft = FTVector2::Zero;
	FTVector2 bottomRight = FTVector2(screenWidth-1, screenHeight-1);

	float nx = (screenPos.x / screenWidth) * 2.f - 1.f;
	float ny = 1 - (screenPos.y / screenHeight) * 2.f;
	float z = 0.0f;

	DirectX::XMVECTOR ndcPos = DirectX::XMVectorSet(nx, ny, z, 1.0f);
	DirectX::XMMATRIX inverseViewProj = DirectX::XMMatrixInverse(nullptr, (GetViewRow() * GetProjRow()));
	DirectX::XMVECTOR worldPos = DirectX::XMVector4Transform(ndcPos, inverseViewProj);
	
	DirectX::SimpleMath::Vector3 vec3(worldPos);
	return FTVector3(vec3.x, vec3.y, vec3.z);
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
	// std::string* actorNames = DBG_NEW std::string[editorElems.size() + 1];
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