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

namespace GenericData
{
	/// @brief A wrapper class to store CSV data.
	class FTCSV :
		public Common::IResource
	{
		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Returns column(horizontal) count.
		virtual size_t GetColumnCount() const;

		/// @brief Returns row(vertical) count.
		virtual size_t GetRowCount() const;

		/// @brief Returns the .csv data stored in vector.
		/// @todo Replace std::vector to Common::FTDS::DynamicArray.
		virtual const Common::FTDS::DynamicArray<int>& Data() const;

	public:
		/// @see FTResource::FTResource
		FTCSV(Common::ResourceData* resDef);
		~FTCSV();

	private:
		Common::ResourceData* mMetaData;

		/// @brief .csv integer data. Useful for making a tiled object.
		Common::FTDS::DynamicArray<int> mData;

		/// @brief Column(horizontal) count.
		size_t mColumnCount;

		/// @brief Row(vertical) count.
		size_t mRowCount;

	private:
		/// @brief Parses .csv, sand stores the integer values to mData.
		void Read(const Common::FTDS::String& path);
	};

	namespace ChunkKey
	{
		namespace CSV
		{
			constexpr const char* CSV = "CSV";
		}
	} // namespace ChunkKey
} // namespace GenericData