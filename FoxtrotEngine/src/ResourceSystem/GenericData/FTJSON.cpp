#include "FTJSON.h"

#include <fstream>

void FTJSON::Read()
{
	std::ifstream ifs(GetRelativePath());
	mData = nlohmann::json::parse(ifs);
}

nlohmann::json& FTJSON::Data() { return mData; }