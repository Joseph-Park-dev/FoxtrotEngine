#include "FTText.h"

#include <FileSystem/FileIOHelper.h>

void FTText::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::TEXT);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::TEXT);
}

void FTText::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::TEXT);
	FTResource::LoadProperties(ifs);
}

void FTText::Process(FTCore* coreInst)
{
}