#include "FTText.h"

#include <FileSystem/FileIOHelper.h>

FTText::FTText(FTResourceDef& resDef)
	: FTResource(resDef)
	, mData(DBG_NEW FTDS::DynamicArray<FTDS::String*>)
{
}

FTText::~FTText()
{
	mData->IterateArray([&](FTDS::String* str) {
		delete str;
	});
	delete mData;
}

void FTText::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTText::FT_TEXT);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTText::FT_TEXT);
}

void FTText::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTText::FT_TEXT);
	FTResource::LoadProperties(ifs);
}

void FTText::Process()
{
	char*		  buf = nullptr;
	std::ifstream ifs(GetFileName().C_Str());
	size_t		  lineCount = GetLineCount(ifs);
	mData->Reserve(lineCount);
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

void FTText::Read(std::ifstream& ifs)
{
	char line[MAX_CHAR_PER_LINE] = {};
	while (std::cin.getline(line, MAX_CHAR_PER_LINE, '\n'))
	{
		FTDS::String* lineStr = DBG_NEW FTDS::String(line);
		mData->PushBack(lineStr);
	}
}