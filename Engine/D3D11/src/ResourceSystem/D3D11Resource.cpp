#include "D3D11Resource.h"

#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/CoreResource.h"
#include "Manager/ResourceManager.h"

void D3D11::D3D11Resource::SaveProperties(std::ofstream& ofs)
{
	Common::FileIOHelper::SaveString(
		ofs,
		Core::ChunkKey::FTResource::FILE_NAME,
		*GetFileName());

	Common::FileIOHelper::SaveString(
		ofs,
		Core::ChunkKey::FTResource::FILE_NAME,
		*GetRelativePath());
}

void D3D11::D3D11Resource::LoadProperties(std::ifstream& ifs)
{
	Common::FTDS::String val;
	Common::FileIOHelper::LoadBasicString(ifs, val);
	val.Clear();
	Common::FileIOHelper::LoadBasicString(ifs, val);
}

D3D11::D3D11Resource::D3D11Resource(Common::FTResourceDef& resDef)
	: Core::FTResource(resDef)
{
}