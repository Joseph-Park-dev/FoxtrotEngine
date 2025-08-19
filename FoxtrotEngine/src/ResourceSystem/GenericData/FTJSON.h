#pragma once
#include "ResourceSystem/FTResource.h"

#include <nlohmann/json.hpp>

class FTJSON :
	public FTResource
{
public:
	void Read();

public:
	nlohmann::json& Data();

private:
	nlohmann::json mData;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;
};

namespace ChunkKey
{
	namespace JSON
	{
		constexpr const char* JSON = "JSON";
	}
} // namespace ChunkKey