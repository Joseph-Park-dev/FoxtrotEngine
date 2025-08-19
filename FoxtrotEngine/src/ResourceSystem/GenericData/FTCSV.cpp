#include "FTCSV.h"

#include <sstream>

#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"

void FTCSV::Read()
{
	if (!mData.empty())
		mData.clear();

	std::ifstream ifs;
	ifs.open(RelativePath().C_Str(), std::fstream::in);
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

UINT FTCSV::GetColumnCount() const { return mColumnCount; }
UINT FTCSV::GetRowCount() const { return mRowCount; }

std::vector<int>& FTCSV::Data() { return mData; }

FTCSV::FTCSV()
	: mData()
	, mColumnCount(0)
	, mRowCount(0)
{
}

FTCSV::~FTCSV()
{
	mData.clear();
}

void FTCSV::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CSV::CSV);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CSV::CSV);
}

void FTCSV::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CSV::CSV);
	FTResource::LoadProperties(ifs);
}

void FTCSV::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->Read();
	this->SetIsProcessed(true);
}

#ifdef FOXTROT_EDITOR
void FTCSV::UpdateUI()
{
}
#endif // FOXTROT_EDITOR