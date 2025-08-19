#include "FTMaterial.h"

#include "Core/FTCore.h"
#include <Renderer/D3D11Utils.h>
#include <Renderer/FoxtrotRenderer.h>

void FTMaterial::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->LoadFromFile();

	FoxtrotRenderer* renderer = coreInst->GetGameRenderer();
	CreatePixelConstBuffer(renderer->GetDevice());

	// All loaded premades are included as default.
	this->SetIsProcessed(true);
}

ComPtr<ID3D11Buffer>& FTMaterial::GetPCBuf()
{
	return mPCBuf;
}