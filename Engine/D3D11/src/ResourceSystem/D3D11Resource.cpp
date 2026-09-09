#include "ResourceSystem/D3D11Resource.h"

#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/CoreResource.h"
#include "Manager/ResourceManager.h"

/// @brief Serializes this object's persistent properties to a .chunk stream.
/// @param ofs Output stream receiving the serialized data.
/// @note Writes to the supplied stream at its current position.
void D3D11::D3D11Resource::SaveProperties(std::ofstream& ofs)
{
	Common::FileIOHelper::SaveString(
		ofs,
		Common::ChunkKey::FTResource::FILE_NAME,
		*GetFileName());

	Common::FileIOHelper::SaveString(
		ofs,
		Common::ChunkKey::FTResource::FILE_NAME,
		*GetRelativePath());
}

/// @brief Restores this object's persistent properties from a .chunk stream.
/// @param ifs Input stream positioned at the expected data; reading advances its position.
/// @note Advances the stream position and updates the destination state.
void D3D11::D3D11Resource::LoadProperties(std::ifstream& ifs)
{
	Common::FTDS::String val;
	Common::FileIOHelper::LoadBasicString(ifs, val);
	val.Clear();
	Common::FileIOHelper::LoadBasicString(ifs, val);
}

/// @brief Initializes resource metadata shared by Direct3D assets.
/// @param resDef Resource definition containing the filename and source path.
/// @note Initializes the :D3D11Resource base or delegates to its constructor.
D3D11::D3D11Resource::D3D11Resource(Common::FTResourceDef& resDef)
	: Common::FTResource(resDef)
{
}
