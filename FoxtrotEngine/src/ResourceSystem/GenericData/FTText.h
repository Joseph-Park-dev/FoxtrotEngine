// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/FTResource.h"

#include <Dynamic/DynamicArray.h>
#include <Static/FTString.h>

/// @brief Maximum number of characters per line.
/// If the line is longer, the exceeding won't be read.
constexpr size_t MAX_CHAR_PER_LINE = 100;

/// @brief A wrapper class for arbitary files holding texts.
/// This can also be used in handling arbitrary files to store & pass
/// file names and relative path.
class FTText :
	public FTResource
{
public:
	/// @see FTResource::SaveProperties()
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @see FTResource::LoadProperties()
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Returns the text data.
	/// The array index equals to line number.
	FTDS::DynamicArray<FTDS::String*>* Data();

public:
	/// @see FTResource::FTResource
	FTText(FTResourceDef& resDef);
	~FTText();

private:
	/// @brief String text data, Loaded line by line.
	/// The array index equals to line number.
	FTDS::DynamicArray<FTDS::String*>* mData;

protected:
	/// @brief Reads the file data as strings and store them into mData.
	virtual void Process() override;

private:
	/// @brief Returns the number of lines of the file.
	size_t GetLineCount(std::ifstream& ifs);

	/// @brief Parses the .txt, and stores the string values to mData.
	void Read(std::ifstream& ifs);
};

namespace ChunkKey
{
	namespace FTText
	{
		constexpr const char* FT_TEXT = "Text";
	}
} // namespace ChunkKey
