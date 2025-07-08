#include "FTMaterial.h"

#include "Core/FTCore.h"

void FTMaterial::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->LoadFromFile();
	// All loaded premades are included as default.
	this->SetIsProcessed(true);
}