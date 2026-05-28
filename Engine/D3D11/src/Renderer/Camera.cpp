#include "Renderer/Camera.h"

#include "FTMath.h"
#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/FTRectArea.h"

#include "Plugin/GetFunc.h"
#include <../Core/include/Plugin/PluginKey.h>

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
		FTVector2 renderSize = mData->GetResolution();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	void Camera::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CAMERA_DATA);
		if (mData->Target)
			Common::FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, mData->Target->GetNameRef());
		else
			Common::FileIOHelper::SaveString(ofs, ChunkKey::TARGET_ACTOR, Common::ChunkKey::NullVal::NULL_OBJECT);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_POSITION, mData->Position);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::CAM_OFFSET, mData->Offset);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::CAM_ZOOM, mData->ZoomFactor);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CAMERA_DATA);
	}

	void Camera::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CAMERA_DATA);
		Common::FileIOHelper::LoadFloat(ifs, mData->ZoomFactor);
		Common::FileIOHelper::LoadVector3(ifs, mData->Offset);

		Common::FileIOHelper::LoadVector3(ifs, mData->Position);
		Common::FTDS::String targetName = {};
		Common::FileIOHelper::LoadBasicString(ifs, targetName);

#ifdef FOXTROT_EDITOR
		if (!mData->Target)
		{
			using FIND_ACTOR = Core::IActor* (*)(Common::FTDS::String&, Core::IActor*);
			mData->Target	 = GetFunc<FIND_ACTOR>(Plugin::Name::CORE_EDITOR, ProcNames::FIND_ACTOR)(targetName, nullptr);
		}
#else
		if (targetName.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
			mTarget = manager->GetCurrentScene()->FindActor(targetName);
#endif // FOXTROT_EDITOR
	}

	Core::CameraData* Camera::Data()
	{
		return mData;
	}

	const Math::FTVector3& Camera::GetPosition() const
	{
		return mData->Position;
	}

	void Camera::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		if (mData->Target)
		{
			Core::Transform* transform = mData->Target->GetTransform();
			FTVector3		 worldPos  = transform->GetWorldPosition();
			// Z axis transformation is controlled independently
			worldPos.z		= mData->Position.z;
			mData->Position = worldPos + mData->Offset;
		}
		outViewMat = FTMatrix4::CreateTranslation(mData->Position);
	}

	void Camera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		const FTVector2 renderSize = mData->GetResolution();
		assert(0 < renderSize.x);
		assert(0 < renderSize.y);

		float worldWidth  = renderSize.x * mData->UnitsPerPixel;
		float worldHeight = renderSize.y * mData->UnitsPerPixel;

		worldWidth /= mData->ZoomFactor;
		worldHeight /= mData->ZoomFactor;

		mData->Aspect = renderSize.x / renderSize.y;

		mData->ViewType == Core::Viewtype::Perspective
			? outProjMat = FTMatrix4::CreatePerspectiveFOV(Math::ToRadians(mData->ProjFOVAngleY), renderSize.x, renderSize.y, mData->NearZ, mData->FarZ)
			: outProjMat = FTMatrix4::CreateOrtho(
				  worldWidth, worldHeight, mData->NearZ, mData->FarZ);
	}

	const Core::Viewtype Camera::GetViewType()
	{
		return mData->ViewType;
	}

	const float Camera::GetProjFOVAngleY()
	{
		return mData->ProjFOVAngleY;
	}

	const float Camera::GetAspectRatio()
	{
		return mData->Aspect;
	}

	const float Camera::GetUnitsPerPixel()
	{
		return mData->UnitsPerPixel;
	}

	const float Camera::GetNearZ()
	{
		return mData->NearZ;
	}

	const float Camera::GetFarZ()
	{
		return mData->FarZ;
	}

	const Math::FTVector3& Camera::GetOffSet() const
	{
		return mData->Offset;
	}

	const float Camera::GetZoomFactor() const
	{
		return mData->ZoomFactor;
	}

	const Math::FTVector2&& Camera::GetResolution() const
	{
		return mData->GetResolution();
	}

	void Camera::SetPosition(const Math::FTVector3& pos)
	{
		mData->Position = pos;
	}

	void Camera::SetViewType(Core::Viewtype viewType)
	{
		mData->ViewType = viewType;
	}

	void Camera::SetTargetActor(Core::IActor* actor)
	{
		mData->Target = actor;
	}

	void Camera::SetOffset(Math::FTVector3 offset)
	{
		mData->Offset = offset;
	}

	Math::FTVector3& Camera::Position()
	{
		return mData->Position;
	}

	Math::FTVector3& Camera::Offset()
	{
		return mData->Offset;
	}

	float& Camera::ZoomFactor()
	{
		return mData->ZoomFactor;
	}

	float& Camera::ZoomDelta()
	{
		return mData->ZoomDelta;
	}

	void Camera::Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit)
	{
		FTVector2 size = renderWindow->GetRenderArea()->GetSize();
		mData->ResX	   = static_cast<unsigned int>(size.x);
		mData->ResY	   = static_cast<unsigned int>(size.y);
		InitializeUnitsPerPixel(pixels, unit);
	}

	void Camera::InitializeUnitsPerPixel(unsigned int pixels, float units)
	{
		mData->UnitsPerPixel = units / static_cast<float>(pixels);
	}

	Camera::Camera(Core::CameraData* data)
		: mData(data)
	{
	}

	Camera::~Camera()
	{
	}
} // namespace D3D11