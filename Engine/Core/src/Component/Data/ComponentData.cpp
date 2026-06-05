#include "ComponentData.h"

#include "FileSystem/FileIOHelper.h"

namespace Core
{
	void ComponentData::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Core::ChunkKey::IS_ACTIVE, IsActive);
		// Common::FileIOHelper::SaveInt(ofs, Core::ChunkKey::UPDATE_ORDER, UpdateOrder);
	}

	void ComponentData::LoadProperties(std::ifstream& ifs)
	{
		// Common::FileIOHelper::LoadInt(ifs, UpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, IsActive);
	}

	void ComponentData::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		chInst->UpdateBoolValue("Is Active", IsActive);
	}
} // namespace Core