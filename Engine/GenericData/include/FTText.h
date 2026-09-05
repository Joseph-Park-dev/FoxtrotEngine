// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

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
		public Common::IResource
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
		Common::FTDS::DynamicArray<Common::FTDS::String*>* Data();

		/// @brief Returns the line string from the data at lineNum.
		/// @return data at index lineNum-1
		Common::FTDS::String* GetLineData(size_t lineNum);

	public:
		/// @see FTResource::FTResource
		FTText(Common::ResourceData* resDef);
		~FTText();

	private:
		Common::ResourceData* mMetaData;

		/// @brief String text data, Loaded line by line.
		/// The array index equals to line number.
		Common::FTDS::DynamicArray<Common::FTDS::String*>* mData;

	private:
		/// @brief Returns the number of lines of the file.
		size_t GetLineCount(std::ifstream& ifs);

		/// @brief Parses the .txt, and stores the string values to mData.
		/// @param count Number of lines to be read.
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