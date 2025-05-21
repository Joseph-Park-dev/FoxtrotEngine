#pragma once
#include "ResourceSystem/FTResource.h"

#include <queue>

class FTCSV :
	public FTResource
{
public:
	// Stores the integer values inside of CSV to result.
	void Read();

public:
	UINT GetColumnCount() const;
	UINT GetRowCount() const;

	std::vector<int>& Data();

public:
	FTCSV();
	~FTCSV() override;

private:
	std::vector<int> mData;
	UINT			 mColumnCount;
	UINT			 mRowCount;

public:
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace CSV
	{
		constexpr const char* CSV = "CSV";
	}
} // namespace ChunkKey