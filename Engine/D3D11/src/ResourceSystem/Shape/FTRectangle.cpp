#include "ResourceSystem/Shape/FTRectangle.h"

#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"
#include "Manager/DebugShapes.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"

namespace D3D11
{
	using namespace Core;
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void FTRectangle::Initialize(D3D11Renderer* renderer)
	{
		FTShape::Initialize(renderer);
		InitializeMesh(renderer->GetDevice(), GeometryGenerator::MakeDebugPoint(Math::FTVector4(1.f, 0.f, 0.f, 1.f)));
	}

	/// @brief Initializes rectangle geometry and its graphics resources.
	/// @note Initializes the :FTRectangle base or delegates to its constructor.
	FTRectangle::FTRectangle()
		: FTShape()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	FTRectangle::~FTRectangle()
	{
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void FTRectangle::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRECTANGLE);
		FileIOHelper::SaveBool(ofs, ChunkKey::FTSHAPE_IS_ACTIVE, GetIsActive());
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRECTANGLE);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void FTRectangle::LoadProperties(std::ifstream& ifs)
	{
		bool isActive = false;

		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTRECTANGLE);
		FileIOHelper::LoadBool(ifs, isActive);

		SetIsActive(isActive);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void FTRectangle::UpdateUI()
	{
		bool isActive = GetIsActive();
		::Editor::UPDATE_BOOL("Render Shape", isActive);
		SetIsActive(isActive);
	}
#endif

	/// @brief Creates rectangle geometry for rendering.
	/// @return Created ftrectangle instance or resource.
	D3D11_API D3D11::FTRectangle* D3D11::CreateFTRectangle()
	{
		return DBG_NEW D3D11::FTRectangle;
	}
} // namespace D3D11
