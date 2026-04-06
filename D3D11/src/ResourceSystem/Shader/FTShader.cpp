#include "FTShader.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"
#include "FTCore.h"
#include "TemplateFunctions.h"
#include "FileSystem/FileTypes.h"
#include "Renderer/D3D11Renderer.h"

namespace D3D11
{
	using namespace Core;

	void FTShader::SetType(ShaderType&& shaderType)
	{
		mType = shaderType;
	}

	void FTShader::LoadMetaData(FTResourceDef& resDef)
	{
		FTDS::String metaPath;
		metaPath.Assign(resDef.Path);
		ReplaceSuffix(metaPath, FileTypes::SHADER, FileTypes::SHADER_META);

		// .shadermeta does not exist, thus the engine is not going to consider the file.
		if (!std::filesystem::exists(std::filesystem::path(metaPath.C_Str())))
			return;

		std::ifstream ifs(metaPath.C_Str());
		LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void FTShader::SaveMetaData()
	{
		FTDS::String metaPath;
		metaPath.Assign(GetRelativePath());
		ReplaceSuffix(metaPath, FileTypes::SHADER, FileTypes::SHADER_META);

		if (!std::filesystem::exists(std::filesystem::path(metaPath.C_Str())))
			Debug::LogError(__LINE__, __FILE__, ".shadermeta does not exist. Creating one...");

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
} // namespace D3D11