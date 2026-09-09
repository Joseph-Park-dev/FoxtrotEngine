#include "FTText.h"

#include "FileSystem/FileIOHelper.h"

namespace GenericData
{
	/// @brief Initializes a resource containing text lines.
	/// @param metaData meta data used by this operation.
	/// @note Initializes the :FTText base or delegates to its constructor.
	FTText::FTText(Common::ResourceData* metaData)
		: Common::FTResource()
		, mData(DBG_NEW Common::FTDS::DynamicArray<Common::FTDS::String*>)
	{
		SetFileName(*metaData->FileName);
		SetRelativePath(*metaData->Path);
		char*		  buf = nullptr;
		std::ifstream ifs(mMetaData->Path->C_Str());
		size_t		  lineCount = GetLineCount(ifs);
		mData->Reserve(lineCount);

		Read(ifs, lineCount);
	}

	/// @brief Releases the resources managed by this instance during destruction.
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

	/// @brief Exposes the stored payload for consumers of this resource or container.
	/// @return Borrowed access to the data.
	Common::FTDS::DynamicArray<Common::FTDS::String*>* FTText::Data() { return mData; }
	/// @brief Returns the line data used by this fttext.
	/// @param lineNum One-based line number in the text resource.
	/// @return Borrowed access to the line data.
	/// @pre lineNum must be between one and the number of stored lines.
	Common::FTDS::String*							   FTText::GetLineData(size_t lineNum) { return mData->At(lineNum - 1); }

	/// @brief Counts newline characters from the current stream position to end of file.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @return Number of newline characters read.
	/// @note Disables whitespace skipping and consumes the stream to end of file.
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

	/// @brief Parses source content into the resource's stored representation.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param count Number of entries to process.
	/// @note Reads from the current stream position; this method does not rewind the stream.
	void FTText::Read(std::ifstream& ifs, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			Common::FTDS::String* line = DBG_NEW Common::FTDS::String;
			Common::FileIOHelper::GetLine(ifs, *line);
			mData->PushBack(line);
		}
	}
} // namespace GenericData
