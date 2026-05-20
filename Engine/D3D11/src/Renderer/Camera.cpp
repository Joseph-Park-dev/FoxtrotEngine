#include "Renderer/Camera.h"

#include "FTMath.h"
#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/FTRectArea.h"

namespace D3D11
{
	using namespace Math;
	Math::FTVector3 Camera::ScreenToWorld(const Math::FTVector2& screenPos)
	{
		FTVector2 ndc	  = ScreenToNDC(screenPos);
		FTVector3 clipPos = FTVector3(ndc.x, ndc.y, 0.0f);

		FTMatrix4 view = FTMatrix4::Identity;
		FTMatrix4 proj = FTMatrix4::Identity;

		GetViewMatrix(view);
		GetProjectionMatrix(proj);

		FTMatrix4 viewProj = view * proj;
		viewProj.Invert();

		return FTVector3::Transform(clipPos, viewProj, 1.0f);
	}

	Math::FTVector2 Camera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return Math::FTVector2();
	}

	Math::FTVector2 Camera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		FTVector2 renderSize = mRenderWindow->GetRenderArea()->GetSize();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	void Camera::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CAMERA_DATA);
		if (mTarget)
			Common::FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, mTarget->GetNameRef());
		else
			Common::FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, Common::ChunkKey::NullVal::NULL_OBJECT);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_POSITION, mPosition);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_OFFSET, mOffset);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::CAM_ZOOM, mZoomFactor);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CAMERA_DATA);
	}

	void Camera::LoadProperties(std::ifstream& ifs, Core::SceneManager* targetActor)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CAMERA_DATA);
		Common::FileIOHelper::LoadFloat(ifs, mZoomFactor);
		Common::FileIOHelper::LoadVector3(ifs, mOffset);

		Common::FileIOHelper::LoadVector3(ifs, mPosition);
		Common::FTDS::String targetName = {};
		Common::FileIOHelper::LoadBasicString(ifs, targetName);

		// #ifdef FOXTROT_EDITOR
		//		if (mTarget)
		//			mTarget = reinterpret_cast<Editor::EditorSceneManager*>(manager)->GetEditorScene()->FindActor(targetName, nullptr);
		// #else
		//		if (targetName.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
		//			mTarget = manager->GetCurrentScene()->FindActor(targetName);
		// #endif // FOXTROT_EDITOR
	}

	const Math::FTVector3& Camera::GetPosition() const
	{
		return mPosition;
	}

	void Camera::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		if (mTarget)
		{
			Core::Transform* transform = mTarget->GetTransform();
			FTVector3		 worldPos  = transform->GetWorldPosition();
			// Z axis transformation is controlled independently
			worldPos.z = mPosition.z;
			mPosition  = worldPos + mOffset;
		}
		outViewMat = FTMatrix4::CreateTranslation(mPosition);
	}

	void Camera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		float	  unitsPerPixel = 1 / mPixelsPerUnit;
		FTVector2 renderSize	= mRenderWindow->GetRenderArea()->GetSize();
		assert(0 < renderSize.x);
		assert(0 < renderSize.y);

		float worldWidth  = renderSize.x * unitsPerPixel;
		float worldHeight = renderSize.y * unitsPerPixel;

		worldWidth /= mZoomFactor;
		worldHeight /= mZoomFactor;

		mAspect = renderSize.x / renderSize.y;

		mViewType == Core::Viewtype::Perspective
			? outProjMat = FTMatrix4::CreatePerspectiveFOV(Math::ToRadians(mProjFOVAngleY), renderSize.x, renderSize.y, mNearZ, mFarZ)
			: outProjMat = FTMatrix4::CreateOrtho(
				  worldWidth, worldHeight, mNearZ, mFarZ);
	}

	const D3D11::D3D11Window* Camera::GetRenderWindow()
	{
		return mRenderWindow;
	}

	const Core::Viewtype Camera::GetViewType()
	{
		return mViewType;
	}

	const float Camera::GetProjFOVAngleY()
	{
		return mProjFOVAngleY;
	}

	const float Camera::GetAspectRatio()
	{
		return mAspect;
	}

	const float Camera::GetPixelsPerUnit()
	{
		return mPixelsPerUnit;
	}

	const float Camera::GetNearZ()
	{
		return mNearZ;
	}

	const float Camera::GetFarZ()
	{
		return mFarZ;
	}

	const Math::FTVector3& Camera::GetOffSet() const
	{
		return mOffset;
	}

	const float Camera::GetZoomFactor() const
	{
		return mZoomFactor;
	}

	void Camera::SetPosition(const Math::FTVector3& pos)
	{
		mPosition = pos;
	}

	void Camera::SetViewType(Core::Viewtype viewType)
	{
		mViewType = viewType;
	}

	void Camera::SetTargetActor(Core::IActor* actor)
	{
		mTarget = actor;
	}

	void Camera::SetOffset(Math::FTVector3 offset)
	{
		mOffset = offset;
	}

	Math::FTVector3& Camera::Position()
	{
		return mPosition;
	}

	Math::FTVector3& Camera::Offset()
	{
		return mOffset;
	}

	float& Camera::ZoomFactor()
	{
		return mZoomFactor;
	}

	float& Camera::ZoomDelta()
	{
		return mZoomDelta;
	}

	void Camera::Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit)
	{
		mRenderWindow = renderWindow;
		InitializePixelsPerUnit(pixels, unit);
	}

	void Camera::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		mPixelsPerUnit = (float)pixels / units;
	}

	Camera::Camera()
		: mRenderWindow(nullptr)
		, mTarget(nullptr)
		, mPosition(FTVector3(0.0f, 0.0f, -5.0f))
		, mProjFOVAngleY(70.f)
		, mNearZ(0.01f)
		, mFarZ(100.0f)
		, mAspect(0.f)
		, mPixelsPerUnit(0.f)
		, mZoomDelta(0.f)
		, mZoomFactor(1.0f)
		, mViewType(Core::Viewtype::Orthographic)
	{
	}

	Camera::~Camera()
	{
	}
} // namespace D3D11