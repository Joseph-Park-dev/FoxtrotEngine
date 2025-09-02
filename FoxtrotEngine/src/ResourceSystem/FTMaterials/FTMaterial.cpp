#include "FTMaterial.h"

#include "Core/FTCore.h"
#include <Renderer/D3D11Utils.h>
#include <Renderer/FoxtrotRenderer.h>

void FTMaterial::SaveProperties(std::ofstream& ofs)
{
}

void FTMaterial::LoadProperties(std::ifstream& ifs)
{
}

ComPtr<ID3D11Buffer>& FTMaterial::GetPCBuf()
{
	return mPCBuf;
}

FTMaterial::FTMaterial(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTResource(resDef)
{
	Process(renderer);
}

void FTMaterial::Process(FoxtrotRenderer* renderer)
{
	if (this->IsProcessed())
		return;

	std::ifstream ifs(GetRelativePath().C_Str());
	this->LoadProperties(ifs);

	CreatePixelConstBuffer(renderer->GetDevice());
	FTResource::Process();
}