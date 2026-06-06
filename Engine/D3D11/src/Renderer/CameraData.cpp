#include "Renderer/CameraData.h"

#include "Renderer/ICamera.h"
#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "FileSystem/FileIOHelper.h"
#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"

namespace Core
{
	using namespace Math;
	Math::FTVector3 Core::CameraData::ScreenToWorld(const Math::FTVector2& screenPos)
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

	Math::FTVector2 Core::CameraData::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		Debug::LogError(__LINE__, __FILE__, "Not implemented");
		return Math::FTVector2();
	}

	Math::FTVector2 Core::CameraData::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		FTVector2 renderSize = GetResolution();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	void Core::CameraData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::CAMERA_DATA);
		if (Target)
			Common::FileIOHelper::SaveString(ofs, Core::ChunkKey::TARGET_ACTOR, Target->GetNameRef());
		else
			Common::FileIOHelper::SaveString(ofs, Core::ChunkKey::TARGET_ACTOR, Common::ChunkKey::NullVal::NULL_OBJECT);
		Common::FileIOHelper::SaveVector3(ofs, Core::ChunkKey::CAM_POSITION, Position);
		Common::FileIOHelper::SaveVector3(ofs, Core::ChunkKey::CAM_OFFSET, Offset);
		Common::FileIOHelper::SaveFloat(ofs, Core::ChunkKey::CAM_ZOOM, ZoomFactor);
		Common::FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::CAMERA_DATA);
	}

	void Core::CameraData::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::CAMERA_DATA);
		Common::FileIOHelper::LoadFloat(ifs, ZoomFactor);
		Common::FileIOHelper::LoadVector3(ifs, Offset);

		Common::FileIOHelper::LoadVector3(ifs, Position);
		Common::FTDS::String targetName = {};
		Common::FileIOHelper::LoadBasicString(ifs, targetName);

#ifdef FOXTROT_EDITOR
		if (!Target)
		{
			using FIND_ACTOR = Common::IActor* (*)(Common::FTDS::String&, Common::IActor*);
			Target	 = GetFunc<FIND_ACTOR>(Plugin::Name::CORE_EDITOR, ProcNames::FIND_ACTOR)(targetName, nullptr);
		}
#else
		if (targetName.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
			mTarget = manager->GetCurrentScene()->FindActor(targetName);
#endif // FOXTROT_EDITOR
	}

	const Math::FTVector3& Core::CameraData::GetPosition() const
	{
		return Position;
	}

	void Core::CameraData::GetViewMatrix(Math::FTMatrix4& outViewMat)
	{
		if (Target)
		{
			Core::Transform* transform = Target->GetData()->Transform;
			FTVector3		 worldPos  = transform->GetWorldPosition();
			// Z axis transformation is controlled independently
			worldPos.z		= Position.z;
			Position = worldPos + Offset;
		}
		outViewMat = FTMatrix4::CreateTranslation(Position);
	}

	void Core::CameraData::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
	{
		const FTVector2 renderSize = GetResolution();
		assert(0 < renderSize.x);
		assert(0 < renderSize.y);

		float worldWidth  = renderSize.x * UnitsPerPixel;
		float worldHeight = renderSize.y * UnitsPerPixel;

		worldWidth /= ZoomFactor;
		worldHeight /= ZoomFactor;

		Aspect = renderSize.x / renderSize.y;

		ViewType == Graphics::Viewtype::Perspective
			? outProjMat = FTMatrix4::CreatePerspectiveFOV(Math::ToRadians(ProjFOVAngleY), renderSize.x, renderSize.y, NearZ, FarZ)
			: outProjMat = FTMatrix4::CreateOrtho(
				  worldWidth, worldHeight, NearZ, FarZ);
	}

	const Graphics::Viewtype Core::CameraData::GetViewType()
	{
		return ViewType;
	}

	const float Core::CameraData::GetProjFOVAngleY()
	{
		return ProjFOVAngleY;
	}

	const float Core::CameraData::GetAspectRatio()
	{
		return Aspect;
	}

	const float Core::CameraData::GetUnitsPerPixel()
	{
		return UnitsPerPixel;
	}

	const float Core::CameraData::GetNearZ()
	{
		return NearZ;
	}

	const float Core::CameraData::GetFarZ()
	{
		return FarZ;
	}

	const Math::FTVector3& Core::CameraData::GetOffSet() const
	{
		return Offset;
	}

	const float Core::CameraData::GetZoomFactor() const
	{
		return ZoomFactor;
	}

	const Math::FTVector2&& Core::CameraData::GetResolution() const
	{
		return Math::FTVector2(ResX, ResY);
	}

	void Core::CameraData::SetPosition(const Math::FTVector3& pos)
	{
		Position = pos;
	}

	void Core::CameraData::SetViewType(Graphics::Viewtype viewType)
	{
		ViewType = viewType;
	}

	void Core::CameraData::SetTargetActor(Common::IActor* actor)
	{
		Target = actor;
	}

	void Core::CameraData::SetOffset(Math::FTVector3 offset)
	{
		Offset = offset;
	}

	void CameraData::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		UnitsPerPixel = units / static_cast<float>(pixels);
	}
} // namespace Core