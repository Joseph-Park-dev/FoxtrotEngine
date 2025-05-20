#include "FTJSON.h"

#include <fstream>

#include "Static/FTString.h"

void FTJSON::Read()
{
	std::ifstream ifs(RelativePath().C_Str());
	mData = nlohmann::json::parse(ifs);
}

nlohmann::json& FTJSON::Data() { return mData; }