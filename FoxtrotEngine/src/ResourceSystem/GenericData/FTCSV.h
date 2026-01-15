// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/FTResource.h"

#include <queue>

/// @brief A wrapper class to store CSV data.
class FTCSV :
	public FTResource
{
public:
	/// @see FTResource::SaveProperties()
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @see FTResource::LoadProperties()
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Returns column(horizontal) count.
	size_t GetColumnCount() const;

	/// @brief Returns row(vertical) count.
	size_t GetRowCount() const;

	/// @brief Returns the .csv data stored in vector.
	/// @todo Replace std::vector to FTDS::DynamicArray.
	const std::vector<int>& Data() const;

public:
	/// @see FTResource::FTResource
	FTCSV(FTResourceDef& resDef);
	~FTCSV();

private:
	/// @brief .csv integer data. Useful for making a tiled object.
	std::vector<int> mData;

	/// @brief Column(horizontal) count.
	size_t mColumnCount;

	/// @brief Row(vertical) count.
	size_t mRowCount;

protected:
	/// @brief Reads the .csv data and store them into mData.
	virtual void Process() override;

private:
	/// @brief Parses .csv, sand stores the integer values to mData.
	void Read();
};

namespace ChunkKey
{
	namespace CSV
	{
		constexpr const char* CSV = "CSV";
	}
} // namespace ChunkKey