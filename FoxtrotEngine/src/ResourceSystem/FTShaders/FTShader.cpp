#include "FTShader.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"

const ShaderType& FTShader::GetType() const { return mType; }
void FTShader::SetType(ShaderType type) { mType = type; }

FTShader::FTShader()
#ifdef FOXTROT_EDITOR
	: mRenderer(nullptr)
#endif // DEBUG
{
}

void FTShader::Process(FTCore* coreInst)
{
	LoadMetaFile();
	CompileShader(coreInst->GetGameRenderer());

#ifdef FOXTROT_EDITOR
	mRenderer = coreInst->GetGameRenderer();
#endif // FOXTROT_EDITOR
}

void FTShader::LoadMetaFile()
{
	FTDS::String metaPath;
	metaPath.Assign(RelativePath());
	ReplaceSuffix(metaPath, FileTypes::SHADER, FileTypes::SHADER_META);
	std::ifstream ifs(metaPath.C_Str());

	if (ifs.good())
		LoadProperties(ifs);
	else
		Debug::LogError(__LINE__, __FILE__, "Failed to load shader meta file");
}

#ifdef FOXTROT_EDITOR
void FTShader::SaveMetaFile()
{
	FTDS::String metaPath;
	metaPath.Assign(RelativePath());
	ReplaceSuffix(metaPath, FileTypes::SHADER, FileTypes::SHADER_META);
	std::ofstream ofs(metaPath.C_Str());

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