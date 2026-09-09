#include "Renderer/Camera.h"

#include "Renderer/CameraData.h"
#include "FTMath.h"
#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/FTRectArea.h"

#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"

namespace D3D11
{
	using namespace Math;
	/// @brief Converts a screen-space position into world coordinates using the camera transforms.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts a screen-space position into world coordinates using the camera transforms.
	Math::FTVector3 Camera::ScreenToWorld(const Math::FTVector2& screenPos)
	{
		return mData->ScreenToWorld(screenPos);
	}

	/// @brief Projects a world-space position into screen coordinates.
	/// @param worldPos Position in world coordinates.
	/// @return Current world to screen.
	Math::FTVector2 Camera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return mData->WorldToScreen(worldPos);
	}

	/// @brief Converts screen coordinates to normalized device coordinates.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts screen coordinates to normalized device coordinates.
	Math::FTVector2 Camera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		return mData->ScreenToNDC(screenPos);
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void Camera::SaveProperties(std::ofstream& ofs)
	{
		mData->SaveProperties(ofs);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void Camera::LoadProperties(std::ifstream& ifs)
	{
		mData->LoadProperties(ifs);
	}

	/// @brief Exposes the stored payload for consumers of this resource or container.
	/// @return Borrowed access to the data.
	D3D11::CameraData* Camera::Data()
	{
		return mData;
	}

	/// @brief Returns the position used by this camera.
	/// @return Borrowed access to the position.
	const Math::FTVector3& Camera::GetPosition() const
	{
		return mData->Position;
	}

	/// @brief Returns the view matrix used by this camera.
	/// @param outViewMat Receives the view matrix.
	void Camera::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		mData->GetViewMatrix(outViewMat);
	}

	/// @brief Returns the projection matrix used by this camera.
	/// @param outProjMat Receives the projection matrix.
	void Camera::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		mData->GetProjectionMatrix(outProjMat);
	}

	/// @brief Returns the view type used by this camera.
	/// @return Current view type.
	const Core::Viewtype Camera::GetViewType()
	{
		return mData->ViewType;
	}

	/// @brief Returns the proj fovangle y used by this camera.
	/// @return Current proj fovangle y.
	const float Camera::GetProjFOVAngleY()
	{
		return mData->ProjFOVAngleY;
	}

	/// @brief Returns the aspect ratio used by this camera.
	/// @return Current aspect ratio.
	const float Camera::GetAspectRatio()
	{
		return mData->Aspect;
	}

	/// @brief Returns the units per pixel used by this camera.
	/// @return Current units per pixel.
	const float Camera::GetUnitsPerPixel()
	{
		return mData->UnitsPerPixel;
	}

	/// @brief Returns the near z used by this camera.
	/// @return Current near z.
	const float Camera::GetNearZ()
	{
		return mData->NearZ;
	}

	/// @brief Returns the far z used by this camera.
	/// @return Current far z.
	const float Camera::GetFarZ()
	{
		return mData->FarZ;
	}

	/// @brief Returns the off set used by this camera.
	/// @return Borrowed access to the off set.
	const Math::FTVector3& Camera::GetOffSet() const
	{
		return mData->Offset;
	}

	/// @brief Returns the zoom factor used by this camera.
	/// @return Current zoom factor.
	const float Camera::GetZoomFactor() const
	{
		return mData->ZoomFactor;
	}

	/// @brief Returns the resolution used by this camera.
	/// @return Current resolution.
	Math::FTVector2 Camera::GetResolution() const
	{
		return mData->GetResolution();
	}

	/// @brief Updates the position used by subsequent operations.
	/// @param pos Replacement position.
	void Camera::SetPosition(const Math::FTVector3& pos)
	{
		mData->Position = pos;
	}

	/// @brief Updates the view type used by subsequent operations.
	/// @param viewType Replacement view type.
	void Camera::SetViewType(Core::Viewtype viewType)
	{
		mData->ViewType = viewType;
	}

	/// @brief Updates the target actor used by subsequent operations.
	/// @param actor Replacement target actor.
	void Camera::SetTargetActor(Core::IActor* actor)
	{
		mData->Target = actor;
	}

	/// @brief Updates the offset used by subsequent operations.
	/// @param offset Replacement offset.
	void Camera::SetOffset(Math::FTVector3 offset)
	{
		mData->Offset = offset;
	}

	/// @brief Exposes the stored spatial position.
	/// @return Borrowed access to the position.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& Camera::Position()
	{
		return mData->Position;
	}

	/// @brief Exposes the drawing offset applied to the stored geometry.
	/// @return Borrowed access to the offset.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& Camera::Offset()
	{
		return mData->Offset;
	}

	/// @brief Applies a multiplicative change to the editor camera zoom.
	/// @return Applies a multiplicative change to the editor camera zoom.
	float& Camera::ZoomFactor()
	{
		return mData->ZoomFactor;
	}

	/// @brief Establishes the conversion from engine world units to pixels.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param units World-space distance or unit count.
	void Camera::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		mData->InitializePixelsPerUnit(pixels, units);
	}

	/// @brief Adjusts the editor camera zoom by a relative amount.
	/// @return Adjusts the editor camera zoom by a relative amount.
	float& Camera::ZoomDelta()
	{
		return mData->ZoomDelta;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderWindow Window associated with the render target.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param unit Unit conversion factor.
	void Camera::Initialize(D3D11::D3D11Window* renderWindow, unsigned int pixels, float unit)
	{
		FTVector2 size = renderWindow->GetRenderArea()->GetSize();
		mData->ResX	   = static_cast<unsigned int>(size.x);
		mData->ResY	   = static_cast<unsigned int>(size.y);
		InitializeUnitsPerPixel(pixels, unit);
	}

	/// @brief Establishes the conversion from pixels to engine world units.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param units World-space distance or unit count.
	void Camera::InitializeUnitsPerPixel(unsigned int pixels, float units)
	{
		mData->InitializePixelsPerUnit(pixels, units);
	}

	/// @brief Initializes camera pose, projection settings, and rendering constants.
	/// @note Initializes the :Camera base or delegates to its constructor.
	Camera::Camera()
		: mData(DBG_NEW D3D11::CameraData)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	Camera::~Camera()
	{
		delete mData;
	}
} // namespace D3D11
