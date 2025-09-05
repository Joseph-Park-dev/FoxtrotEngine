#include "FTShader.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"
#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"

FTShader::FTShader(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTResource(resDef)
{
}

void FTShader::SetType(ShaderType&& shaderType)
{
	mType = shaderType;
}

#ifdef FOXTROT_EDITOR
void FTShader::SaveMetaData()
{
	FTDS::String metaPath;
	metaPath.Assign(GetRelativePath());
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
#endif // FOXTROT_EDITOR