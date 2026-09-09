#include "FTCSV.h"

#include <sstream>
#include <queue>

#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace GenericData
{
	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void FTCSV::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
		mMetaData->SaveProperties(ofs);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void FTCSV::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
		mMetaData->LoadProperties(ifs);
	}

	/// @brief Returns the column count used by this ftcsv.
	/// @return Current column count.
	size_t FTCSV::GetColumnCount() const { return mColumnCount; }
	/// @brief Returns the row count used by this ftcsv.
	/// @return Current row count.
	size_t FTCSV::GetRowCount() const { return mRowCount; }

	/// @brief Exposes the stored payload for consumers of this resource or container.
	/// @return Borrowed access to the data.
	const Common::FTDS::DynamicArray<int>& FTCSV::Data() const { return mData; }

	/// @brief Initializes a resource containing parsed integer CSV data.
	/// @param metaData meta data used by this operation.
	/// @note Initializes the :FTCSV base or delegates to its constructor.
	FTCSV::FTCSV(Common::ResourceData* metaData)
		: Common::FTResource()
		, mData()
		, mColumnCount(0)
		, mRowCount(0)
	{
		SetFileName(*metaData->FileName);
		SetRelativePath(*metaData->Path);
		if (!mData.IsEmpty())
			return;

		this->Read(*metaData->Path);
	}

	/// @brief Releases the resources managed by this instance during destruction.
	FTCSV::~FTCSV()
	{
		mData.Clear();
	}

	/// @brief Parses source content into the resource's stored representation.
	/// @param path Filesystem path of the resource or project.
	void FTCSV::Read(const Common::FTDS::String& path)
	{
		if (!mData.IsEmpty())
			mData.Clear();

		std::ifstream ifs;
		ifs.open(path.C_Str(), std::fstream::in);
		assert(ifs);

		std::string		line;
		int				val;
		std::queue<int> resultBuf;

		UINT column = 0;
		UINT row	= 0;
		while (std::getline(ifs, line))
		{
			std::stringstream ss(line);
			// Create a stringstream of the current line
			// Extract each integer
			while (ss >> val)
			{
				resultBuf.push(val);
				if (ss.peek() == ',')
					ss.ignore();
				++column;
			}
			++row;
		}
		if (row != 0)
			column /= row;

		mColumnCount = column;
		mRowCount	 = row;

		mData.Reserve(resultBuf.size());

		while (!resultBuf.empty())
		{
			mData.PushBack(resultBuf.front());
			resultBuf.pop();
		}

		ifs.close();
	}
} // namespace GenericData
