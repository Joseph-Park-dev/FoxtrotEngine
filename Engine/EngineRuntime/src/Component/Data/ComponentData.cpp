#include "Component/Data/ComponentData.h"

#include "FileSystem/FileIOHelper.h"

namespace Common
{
	void ComponentData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Common::ChunkKey::IS_ACTIVE, IsActive);
		// Common::FileIOHelper::SaveInt(ofs, Core::ChunkKey::UPDATE_ORDER, UpdateOrder);
	}

	void ComponentData::LoadProperties(std::ifstream& ifs)
	{
		// Common::FileIOHelper::LoadInt(ifs, UpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, IsActive);
	}
} // namespace Common