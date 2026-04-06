#include "D3D11Resource.h"

#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/CoreResource.h"
#include "Manager/ResourceManager.h"

void D3D11::D3D11Resource::SaveProperties(std::ofstream& ofs)
{
	Core::FileIOHelper::SaveString(
		ofs,
		Core::ChunkKey::FTResource::FILE_NAME,
		D3D11::RES_NAME(D3D11Resource, this));
}

void D3D11::D3D11Resource::LoadProperties(std::ifstream& ifs)
{
	Core::FTDS::String fileName;
	Core::FileIOHelper::LoadBasicString(
		ifs,
		fileName);
}