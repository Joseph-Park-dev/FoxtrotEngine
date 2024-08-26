#include "FoxtrotEngine/ResourceSystem/FTResource.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

void FTResource::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["RelativePath"], mRelativePath);
}