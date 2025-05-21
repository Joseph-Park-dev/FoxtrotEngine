#include "FTJSON.h"

#include <fstream>

#include "Core/FTCore.h"
#include "Static/FTString.h"

void FTJSON::Read()
{
	std::ifstream ifs(RelativePath().C_Str());
	mData = nlohmann::json::parse(ifs);
}

nlohmann::json& FTJSON::Data() { return mData; }

void FTJSON::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->Read();

	this->SetIsProcessed(true);
}