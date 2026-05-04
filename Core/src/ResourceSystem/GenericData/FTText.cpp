#include "FTText.h"

#include "FileSystem/FileIOHelper.h"

namespace Core
{
	FTText::FTText(Common::FTResourceDef& resDef)
		: CoreResource(resDef)
		, mData(DBG_NEW Common::FTDS::DynamicArray<Common::FTDS::String*>)
	{
		char*		  buf = nullptr;
		std::ifstream ifs(resDef.Path);
		size_t		  lineCount = GetLineCount(ifs);
		mData->Reserve(lineCount);

		Read(ifs, lineCount);
	}

	FTText::~FTText()
	{
		mData->IterateArray([&](Common::FTDS::String* str) {
			delete str;
		});
		delete mData;
	}

	// void FTText::SaveProperties(std::ofstream& ofs)
	//{
	//	Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTText::FT_TEXT);
	//	FTResource::SaveProperties(ofs);
	//	Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTText::FT_TEXT);
	// }
	//
	// void FTText::LoadProperties(std::ifstream& ifs)
	//{
	//	Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTText::FT_TEXT);
	//	FTResource::LoadProperties(ifs);
	// }

	Common::FTDS::DynamicArray<Common::FTDS::String*>* FTText::Data() { return mData; }
	Common::FTDS::String*							   FTText::GetLineData(size_t lineNum) { return mData->At(lineNum - 1); }

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

	void FTText::Read(std::ifstream& ifs, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			Common::FTDS::String* line = DBG_NEW Common::FTDS::String;
			Common::FileIOHelper::GetLine(ifs, *line);
			mData->PushBack(line);
		}
	}
} // namespace Core