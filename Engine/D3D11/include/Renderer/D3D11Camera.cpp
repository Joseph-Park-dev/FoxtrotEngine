#include "Renderer/D3D11Camera.h"

#include "FTMath.h"
#include "Actor/Actor.h"
#include "Actor/Transform.h"

namespace D3D11
{
	using namespace Math;
	Math::FTVector3 D3D11::D3D11Camera::ScreenToWorld(const Math::FTVector2& screenPos) const
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

	Math::FTVector2 D3D11::D3D11Camera::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		return Math::FTVector2();
	}

	Math::FTVector2 D3D11::D3D11Camera::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		FTVector2 renderSize = GetResolution();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	void D3D11Camera::GetViewMatrix(Math::FTMatrix4& outViewMat) const
	{
		if (mTarget)
		{
			Core::Transform* transform = mTarget->GetTransform();
			FTVector3		 targetPos = transform->GetWorldPosition();
			// Z axis transformation is controlled independently
			targetPos.z = mPosition.z;
			mPosition	= targetPos + mOffset;
		}
		outViewMat = FTMatrix4::CreateTranslation(mPosition);
	}

	void D3D11Camera::GetProjectionMatrix(Math::FTMatrix4& outProjMat) const
	{
		float	  unitsPerPixel = 1 / mPixelsPerUnit;
		FTVector2 renderSize	= GetResolution();
		assert(0 < renderSize.x);
		assert(0 < renderSize.y);

		float worldWidth  = renderSize.x * unitsPerPixel;
		float worldHeight = renderSize.y * unitsPerPixel;

		worldWidth /= mZoomFactor;
		worldHeight /= mZoomFactor;

		mAspect = renderSize.x / renderSize.y;

		return mViewType == Viewtype::Perspective
			? FTMatrix4::CreatePerspectiveFOV(Math::ToRadians(mProjFOVAngleY), GetResolution().x, GetResolution().y, mNearZ, mFarZ)
			: FTMatrix4::CreateOrtho(
				  worldWidth, worldHeight, mNearZ, mFarZ);
	}

	D3D11Camera::D3D11Camera()
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
		, mViewType(Viewtype::Orthographic)
	{
	}
} // namespace D3D11