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

namespace GenericData
{
	/// @brief A wrapper class to store CSV data.
	class FTCSV :
		public Common::FTResource
	{
		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Returns column(horizontal) count.
		/// @return Current column count.
		virtual size_t GetColumnCount() const;

		/// @brief Returns row(vertical) count.
		/// @return Current row count.
		virtual size_t GetRowCount() const;

		/// @brief Returns the .csv data stored in vector.
		/// @todo Replace std::vector to Common::FTDS::DynamicArray.
		/// @return Borrowed access to the data.
		virtual const Common::FTDS::DynamicArray<int>& Data() const;

	public:
		/// @brief Initializes a resource containing parsed integer CSV data.
		/// @see FTResource::FTResource
		/// @param resDef Resource definition containing the filename and source path.
		FTCSV(Common::ResourceData* resDef);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTCSV();

	private:


		/// @brief .csv integer data. Useful for making a tiled object.
		Common::FTDS::DynamicArray<int> mData;

		/// @brief Column(horizontal) count.
		size_t mColumnCount;

		/// @brief Row(vertical) count.
		size_t mRowCount;

	private:
		/// @brief Parses .csv, sand stores the integer values to mData.
		/// @param path Filesystem path of the resource or project.
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
