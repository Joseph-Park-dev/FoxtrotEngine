#include "Component/Data/ComponentData.h"

#include "FileSystem/FileIOHelper.h"

namespace Common
{
	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void ComponentData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Common::ChunkKey::IS_ACTIVE, IsActive);
		// Common::FileIOHelper::SaveInt(ofs, Core::ChunkKey::UPDATE_ORDER, UpdateOrder);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void ComponentData::LoadProperties(std::ifstream& ifs)
	{
		// Common::FileIOHelper::LoadInt(ifs, UpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, IsActive);
	}
} // namespace Common
