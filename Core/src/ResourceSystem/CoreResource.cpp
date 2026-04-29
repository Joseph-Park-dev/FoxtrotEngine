#include "CoreResource.h"

#include "FileSystem/FileIOHelper.h"

void Core::CoreResource::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::SaveString(ofs, ChunkKey::FTResource::FILE_NAME, *GetFileName());
	FileIOHelper::SaveString(ofs, ChunkKey::FTResource::FILE_NAME, *GetRelativePath());
}

void Core::CoreResource::LoadProperties(std::ifstream& ifs)
{
	FTDS::String val;
	FileIOHelper::LoadBasicString(ifs, val);
	val.Clear();
	FileIOHelper::LoadBasicString(ifs, val);
}

Core::CoreResource::CoreResource(FTResourceDef& resDef)
	: FTResource(resDef)
{
}
