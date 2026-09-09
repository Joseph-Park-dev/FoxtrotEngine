#include "Renderer/CameraData.h"

#include "Renderer/ICamera.h"
#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "FileSystem/FileIOHelper.h"
#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"
#include "FileSystem/DLLPath.h"

namespace D3D11
{
	using namespace Math;
	/// @brief Converts a screen-space position into world coordinates using the camera transforms.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts a screen-space position into world coordinates using the camera transforms.
	Math::FTVector3 CameraData::ScreenToWorld(const Math::FTVector2& screenPos)
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

	/// @brief Projects a world-space position into screen coordinates.
	/// @param worldPos Position in world coordinates.
	/// @return Current world to screen.
	Math::FTVector2 CameraData::WorldToScreen(const Math::FTVector3& worldPos) const
	{
		Common::Debug::LogError(__LINE__, __FILE__, "Not implemented");
		return Math::FTVector2();
	}

	/// @brief Converts screen coordinates to normalized device coordinates.
	/// @param screenPos Position in screen coordinates.
	/// @return Converts screen coordinates to normalized device coordinates.
	Math::FTVector2 CameraData::ScreenToNDC(const Math::FTVector2& screenPos) const
	{
		FTVector2 renderSize = GetResolution();
		FTVector2 ndc		 = FTVector2::Zero;

		ndc.x = (screenPos.x / renderSize.x) * 2.f - 1.0f;
		ndc.y = 1.0f - (screenPos.y / renderSize.y) * 2.f;
		return ndc;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void CameraData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, Graphics::ChunkKey::CAMERA_DATA);
		if (Target)
			Common::FileIOHelper::SaveString(ofs, Graphics::ChunkKey::TARGET_ACTOR, Target->GetNameRef());
		else
			Common::FileIOHelper::SaveString(ofs, Graphics::ChunkKey::TARGET_ACTOR, Common::ChunkKey::NullVal::NULL_OBJECT);
		Common::FileIOHelper::SaveVector3(ofs, Graphics::ChunkKey::CAM_POSITION, Position);
		Common::FileIOHelper::SaveVector3(ofs, Graphics::ChunkKey::CAM_OFFSET, Offset);
		Common::FileIOHelper::SaveFloat(ofs, Graphics::ChunkKey::CAM_ZOOM, ZoomFactor);
		Common::FileIOHelper::EndDataPackSave(ofs, Graphics::ChunkKey::CAMERA_DATA);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void CameraData::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, Graphics::ChunkKey::CAMERA_DATA);
		Common::FileIOHelper::LoadFloat(ifs, ZoomFactor);
		Common::FileIOHelper::LoadVector3(ifs, Offset);

		Common::FileIOHelper::LoadVector3(ifs, Position);
		Common::FTDS::String targetName = {};
		Common::FileIOHelper::LoadBasicString(ifs, targetName);

#ifdef FOXTROT_EDITOR
		if (!Target)
		{
			using FindActorFn = Common::IActor* (*)(const char*, Common::IActor*);
			Target			  = Core::GetFunc<FindActorFn>(Common::DLLPath::CORE, "FindActor")(targetName.C_Str(), nullptr);
		}
#else
		if (targetName.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
			Target = Core::GetFunc<Common::IActor* (*)(const char*, Common::IActor*)>(Common::DLLPath::CORE, "FindActor")(targetName.C_Str(), nullptr);
#endif // FOXTROT_EDITOR
	}

	/// @brief Returns the position used by this camera data.
	/// @return Borrowed access to the position.
	const Math::FTVector3& CameraData::GetPosition() const
	{
		return Position;
	}

	/// @brief Returns the view matrix used by this camera data.
	/// @param outViewMat Receives the view matrix.
	void CameraData::GetViewMatrix(Math::FTMatrix4& outViewMat)
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

	/// @brief Returns the projection matrix used by this camera data.
	/// @param outProjMat Receives the projection matrix.
	void CameraData::GetProjectionMatrix(Math::FTMatrix4& outProjMat)
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

	/// @brief Returns the view type used by this camera data.
	/// @return Current view type.
	const Graphics::Viewtype CameraData::GetViewType()
	{
		return ViewType;
	}

	/// @brief Returns the proj fovangle y used by this camera data.
	/// @return Current proj fovangle y.
	const float CameraData::GetProjFOVAngleY()
	{
		return ProjFOVAngleY;
	}

	/// @brief Returns the aspect ratio used by this camera data.
	/// @return Current aspect ratio.
	const float CameraData::GetAspectRatio()
	{
		return Aspect;
	}

	/// @brief Returns the units per pixel used by this camera data.
	/// @return Current units per pixel.
	const float CameraData::GetUnitsPerPixel()
	{
		return UnitsPerPixel;
	}

	/// @brief Returns the near z used by this camera data.
	/// @return Current near z.
	const float CameraData::GetNearZ()
	{
		return NearZ;
	}

	/// @brief Returns the far z used by this camera data.
	/// @return Current far z.
	const float CameraData::GetFarZ()
	{
		return FarZ;
	}

	/// @brief Returns the off set used by this camera data.
	/// @return Borrowed access to the off set.
	const Math::FTVector3& CameraData::GetOffSet() const
	{
		return Offset;
	}

	/// @brief Returns the zoom factor used by this camera data.
	/// @return Current zoom factor.
	const float CameraData::GetZoomFactor() const
	{
		return ZoomFactor;
	}

	/// @brief Returns the resolution used by this camera data.
	/// @return Current resolution.
	Math::FTVector2 CameraData::GetResolution() const
	{
		return Math::FTVector2(ResX, ResY);
	}

	/// @brief Updates the position used by subsequent operations.
	/// @param pos Replacement position.
	void CameraData::SetPosition(const Math::FTVector3& pos)
	{
		Position = pos;
	}

	/// @brief Updates the view type used by subsequent operations.
	/// @param viewType Replacement view type.
	void CameraData::SetViewType(Graphics::Viewtype viewType)
	{
		ViewType = viewType;
	}

	/// @brief Updates the target actor used by subsequent operations.
	/// @param actor Replacement target actor.
	void CameraData::SetTargetActor(Common::IActor* actor)
	{
		Target = actor;
	}

	/// @brief Updates the offset used by subsequent operations.
	/// @param offset Replacement offset.
	void CameraData::SetOffset(Math::FTVector3 offset)
	{
		Offset = offset;
	}

	/// @brief Establishes the conversion from engine world units to pixels.
	/// @param pixels Pixel data or pixel dimensions.
	/// @param units World-space distance or unit count.
	void CameraData::InitializePixelsPerUnit(unsigned int pixels, float units)
	{
		UnitsPerPixel = units / static_cast<float>(pixels);
	}
} // namespace D3D11
