#include "Renderer/Camera.h"

#include "Renderer/CameraData.h"
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
		return mData->ScreenToWorld(screenPos);
	}

	Math::FTVector2 Camera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return mData->WorldToScreen(worldPos);
	}

	Math::FTVector2 Camera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		return mData->ScreenToNDC(screenPos);
	}

	void Camera::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	void Camera::LoadProperties(std::ifstream& ifs)
	{
		mData->LoadProperties(ifs);
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
		mData->GetViewMatrix(outViewMat);
	}

	void Camera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		mData->GetProjectionMatrix(outProjMat);
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

	void Camera::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		mData->InitializePixelsPerUnit(pixels, units);
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
		mData->InitializePixelsPerUnit(pixels, units);
	}

	Camera::Camera()
		: mData(DBG_NEW Core::CameraData)
	{
	}

	Camera::~Camera()
	{
	}
} // namespace D3D11