#include "FTShader.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"

const ShaderType& FTShader::GetType() const { return mType; }
void FTShader::SetType(ShaderType type) { mType = type; }

FTShader::FTShader()
#ifdef DEBUG
	: mRenderer(nullptr)
#endif // DEBUG
{
}

#ifdef FOXTROT_EDITOR
void FTShader::SaveMetaFile()
{
	std::ofstream ofs(GetRelativePath());

	if (ofs)
	{
		SaveProperties(ofs);
		FileIOHelper::SaveBufferToFile(ofs);
	}
	else
		Debug::LogError(__LINE__, __FILE__, "Failed to save shader meta file");
}

FoxtrotRenderer* FTShader::GetRenderer()
{
	return mRenderer;
}

FTShader::FTShader(FoxtrotRenderer* renderer)
	: mRenderer(renderer)
{
}
#endif // FOXTROT_EDITOR