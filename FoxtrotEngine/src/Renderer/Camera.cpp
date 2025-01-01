#include "Renderer/Camera.h"

#include "directxtk/SimpleMath.h"

#include "Core/FTCore.h"
#include "Managers/KeyInputManager.h"
#include "Managers/SceneManager.h"
#include "Scenes/Scene.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorElement.h"
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
	, mTarget(nullptr)
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
	float unitsPerPixel = 1 / mPixelsPerUnit;
	if (mTarget)
	{
		Transform* transform = mTarget->GetTransform();
		mPosition = ConvertToCenter(transform->GetWorldPosition() + mOffset).GetDXVec3();
		mYaw = -transform->GetRotation().y;
		mPitch = transform->GetRotation().x;
	}
	return
		Matrix::CreateTranslation(mPosition) *
		Matrix::CreateRotationY(mYaw) *
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

void Camera::SetTargetActor(Actor* actor)
{
	mTarget = actor;
}

void Camera::SetViewType(Viewtype viewType)
{
	mViewType = viewType;
}

void Camera::SetOffset(FTVector2 offset)
{
	mOffset = offset;
}

void Camera::InitializePixelsPerUnit(float pixels, float units)
{
	mPixelsPerUnit = pixels / units;
}

FTVector3 Camera::ConvertToCenter(FTVector3 topLeftPos)
{
	FTVector3 pos = topLeftPos * FTVector3(-1.f, 1.f, 1.0f);
	float unitsPerPixel = 1 / mPixelsPerUnit;
	float worldWidth = mRenderer->GetRenderWidth() * unitsPerPixel;
	float worldHeight = mRenderer->GetRenderHeight() * unitsPerPixel;
	pos += FTVector3(worldWidth / 2, worldHeight / 2, 0.f);
	return pos;
}

FTVector3 Camera::ConvertToTopLeft(FTVector3 centerPos)
{
	float unitsPerPixel = 1 / mPixelsPerUnit;
	float worldWidth = mRenderer->GetRenderWidth() * unitsPerPixel;
	float worldHeight = mRenderer->GetRenderHeight() * unitsPerPixel;
	FTVector3 pos = centerPos - FTVector3(worldWidth / 2, worldHeight / 2, 0.f);
	pos = pos * FTVector3(-1.f, 1.f, 1.0f);
	return pos;
}

void Camera::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::CAMERA_DATA);
	if(mTarget)
		FileIOHelper::SaveString(ofs, ChunkKeys::TARGET_ACTOR, mTarget->GetName());
	else
		FileIOHelper::SaveString(ofs, ChunkKeys::TARGET_ACTOR, ChunkKeys::NULL_OBJ);
	FileIOHelper::SaveVector3(ofs, ChunkKeys::OFFSET, mOffset);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::CAMERA_DATA);
}

void Camera::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::CAMERA_DATA);
	FileIOHelper::LoadVector3(ifs, mOffset);
	std::string targetActor = {};
	FileIOHelper::LoadBasicString(ifs, targetActor);

#ifdef FOXTROT_EDITOR
	if (targetActor != ChunkKeys::NULL_OBJ)
		mTarget = EditorSceneManager::GetInstance()->GetEditorScene()->FindActor(targetActor);
#else
	if (targetActor != ChunkKeys::NULL_OBJ)
		mTarget = SceneManager::GetInstance()->GetCurrentScene()->FindActor(targetActor);
#endif // FOXTROT_EDITOR

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
	FTVector3 pos = ConvertToTopLeft(FTVector3(mPosition));
	CommandHistory::GetInstance()->UpdateVector3Value("Look-At Position", pos, LOOKAT_MODSPEED);
	CommandHistory::GetInstance()->UpdateFloatValue("Look-At Yaw", &mYaw, LOOKAT_MODSPEED);
	CommandHistory::GetInstance()->UpdateFloatValue("Look-At Pitch", &mPitch, LOOKAT_MODSPEED);
	mPosition = ConvertToCenter(pos).GetDXVec3();

	//// Updating screen center since the camera position is moved
	// FTVector2 diff = updatedLookAtPos - lookAtPos;
	// FTVector2 screenCenter = Camera2D::GetInstance()->GetScreenCenter();
	// Camera2D::GetInstance()->SetScreenCenter(screenCenter + diff);

	// Set Target
	EditorScene* editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::vector<Actor*>* editorElems = editorScene->GetActors();
	std::string* actorNames = DBG_NEW std::string[editorScene->GetActorCount() + 1];
	actorNames[0] = "None";
	size_t idx = 1;
	static size_t currIdx;

	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		if (0 < editorElems[i].size())
		{
			for (size_t j = 0; j < editorElems[i].size(); ++j)
			{
				actorNames[idx] = dynamic_cast<EditorElement*>(editorElems[i][j])->GetName();
				++idx;
			}
		}
	}
	const char* comboPreview = actorNames[currIdx].c_str();
	if (ImGui::BeginCombo("Target Actor", comboPreview))
	{
		for (size_t i = 0; i < idx; ++i)
		{
			if (ImGui::Selectable(actorNames[i].c_str()))
			{
				currIdx = i;
				if (currIdx == 0)
					mTarget = nullptr;
				else
					mTarget = editorScene->FindActor(actorNames[currIdx]);
			}
		}
		ImGui::EndCombo();
	}
	delete[] actorNames;
	CommandHistory::GetInstance()->UpdateVector3Value("Offset from target", mOffset, LOOKAT_MODSPEED);
	ImGui::End();
}
#endif // FOXTROT_EDITOR