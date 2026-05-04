#include "FTCSV.h"

#include <sstream>

#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"

// void FTCSV::SaveProperties(std::ofstream& ofs)
//{
//	Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
//	FTResource::SaveProperties(ofs);
//	Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);
// }
//
// void FTCSV::LoadProperties(std::ifstream& ifs)
//{
//	Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
//	FTResource::LoadProperties(ifs);
// }

namespace Core
{
	size_t FTCSV::GetColumnCount() const { return mColumnCount; }
	size_t FTCSV::GetRowCount() const { return mRowCount; }

	const std::vector<int>& FTCSV::Data() const { return mData; }

	FTCSV::FTCSV(Common::FTResourceDef& resDef)
		: CoreResource(resDef)
		, mData()
		, mColumnCount(0)
		, mRowCount(0)
	{
		if (!mData.empty())
			return;

		this->Read(resDef.Path);
	}

	FTCSV::~FTCSV()
	{
		mData.clear();
	}

	void FTCSV::Read(const Common::FTDS::String& path)
	{
		if (!mData.empty())
			mData.clear();

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

		mData.reserve(resultBuf.size());

		while (!resultBuf.empty())
		{
			mData.push_back(resultBuf.front());
			resultBuf.pop();
		}

		ifs.close();
	}
} // namespace Core