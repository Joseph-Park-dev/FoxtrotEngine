#include "CoreResource.h"

#include "FileSystem/FileIOHelper.h"

void Core::CoreResource::SaveProperties(std::ofstream& ofs)
{
	Common::FileIOHelper::SaveString(ofs, Common::ChunkKey::FTResource::FILE_NAME, *GetFileName());
	Common::FileIOHelper::SaveString(ofs, Common::ChunkKey::FTResource::FILE_NAME, *GetRelativePath());
}

void Core::CoreResource::LoadProperties(std::ifstream& ifs)
{
	Common::FTDS::String val;
	Common::FileIOHelper::LoadBasicString(ifs, val);
	val.Clear();
	Common::FileIOHelper::LoadBasicString(ifs, val);
}

Core::CoreResource::CoreResource(Common::FTResourceDef& resDef)
	: FTResource(resDef)
{
}
