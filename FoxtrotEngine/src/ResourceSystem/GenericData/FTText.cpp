#include "FTText.h"

#include <FileSystem/FileIOHelper.h>

FTText::FTText(FTResource& resDef)
	: mData(DBG_NEW FTDS::DynamicArray<FTDS::String>)
{
}

FTText::~FTText()
{
	delete mData;
}

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

void FTText::Process()
{
	char*		  buf = nullptr;
	std::ifstream ifs(GetFileName().C_Str());
	size_t lineCount = GetLineCount(ifs);
	mData->Reserve(lineCount);

	std::cin.getline(buf, MAX_CHAR_PER_LINE, '\n');
	mData->PushBack(buf);
}

size_t FTText::GetLineCount(std::ifstream& ifs)
{
	// new lines will be skipped unless we stop it from happening:
	ifs.unsetf(std::ios_base::skipws);

	// count the newlines with an algorithm specialized for counting:
	size_t count = std::count(
		std::istream_iterator<char>(ifs),
		std::istream_iterator<char>(),
		'\n');

	return count;
}
