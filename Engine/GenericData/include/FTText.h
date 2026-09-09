// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/FTResource.h"

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/FTString.h"

namespace GenericData
{
	/// @brief Maximum number of characters per line.
	/// If the line is longer, the exceeding won't be read.
	constexpr size_t MAX_CHAR_PER_LINE = 100;

	/// @brief A wrapper class for arbitary files holding texts.
	/// This can also be used in handling arbitrary files to store & pass
	/// file names and relative path.
	class FTText :
		public Common::FTResource
	{
		// public:
		//	/// @see FTResource::SaveProperties()
		//	virtual void SaveProperties(std::ofstream& ofs) override;
		//
		//	/// @see FTResource::LoadProperties()
		//	virtual void LoadProperties(std::ifstream& ifs) override;
	public:
		/// @brief Returns the text data.
		/// The array index equals to line number.
		/// @return Borrowed access to the data.
		Common::FTDS::DynamicArray<Common::FTDS::String*>* Data();

		/// @brief Returns the line string from the data at lineNum.
		/// @return data at index lineNum-1
		/// @param lineNum One-based line number in the text resource.
		/// @pre lineNum must be between one and the number of stored lines.
		Common::FTDS::String* GetLineData(size_t lineNum);

	public:
		/// @brief Initializes a resource containing text lines.
		/// @see FTResource::FTResource
		/// @param resDef Resource definition containing the filename and source path.
		FTText(Common::ResourceData* resDef);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTText();

	private:


		/// @brief String text data, Loaded line by line.
		/// The array index equals to line number.
		Common::FTDS::DynamicArray<Common::FTDS::String*>* mData;

	private:
		/// @brief Returns the number of lines of the file.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @return Number of newline characters read.
		/// @note Disables whitespace skipping and consumes the stream to end of file.
		size_t GetLineCount(std::ifstream& ifs);

		/// @brief Parses the .txt, and stores the string values to mData.
		/// @param count Number of lines to be read.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Reads from the current stream position; this method does not rewind the stream.
		void Read(std::ifstream& ifs, size_t count);
	};

	namespace ChunkKey
	{
		namespace FTText
		{
			constexpr const char* FT_TEXT = "Text";
		}
	} // namespace ChunkKey
} // namespace GenericData
