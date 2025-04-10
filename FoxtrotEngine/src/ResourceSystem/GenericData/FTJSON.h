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
};

namespace ChunkKey
{
	namespace JSON
	{
		constexpr const char* JSON = "JSON";
	}
} // namespace ChunkKey