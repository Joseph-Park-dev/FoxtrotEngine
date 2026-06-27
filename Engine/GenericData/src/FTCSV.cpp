#include "FTCSV.h"

#include <sstream>
#include <queue>

#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace GenericData
{
	void FTCSV::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
		mMetaData->SaveProperties(ofs);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);
	}

	void FTCSV::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
		mMetaData->LoadProperties(ifs);
	}

	size_t FTCSV::GetColumnCount() const { return mColumnCount; }
	size_t FTCSV::GetRowCount() const { return mRowCount; }

	const Common::FTDS::DynamicArray<int>& FTCSV::Data() const { return mData; }

	FTCSV::FTCSV(Common::ResourceData* metaData)
		: mMetaData(metaData)
		, mData()
		, mColumnCount(0)
		, mRowCount(0)
	{
		if (!mData.IsEmpty())
			return;

		this->Read(*metaData->Path);
	}

	FTCSV::~FTCSV()
	{
		mData.Clear();
	}

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