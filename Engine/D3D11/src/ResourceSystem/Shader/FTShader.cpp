#include "ResourceSystem/Shader/FTShader.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "Debugging/DebugFuncs.h"
#include "TemplateFunctions.h"
#include "FileSystem/FileTypes.h"
#include "Renderer/D3D11Renderer.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;

	/// @brief Initializes shader metadata and compilation state.
	/// @param resDef Resource definition containing the filename and source path.
	/// @note Initializes the :FTShader base or delegates to its constructor.
	FTShader::FTShader(Common::FTResourceDef& resDef)
		: D3D11::D3D11Resource(resDef)
	{
	}

	/// @brief Updates the type used by subsequent operations.
	/// @param shaderType Replacement type.
	void FTShader::SetType(ShaderType&& shaderType)
	{
		mType = shaderType;
	}

	/// @brief Restores resource metadata from its serialized representation.
	/// @param resDef Resource definition containing the filename and source path.
	void FTShader::LoadMetaData(Common::FTResourceDef& resDef)
	{
		Common::FTDS::String metaPath;
		metaPath.Assign(resDef.Path);
		ReplaceSuffix(metaPath, Common::FileTypes::SHADER, Common::FileTypes::SHADER_META);

		// .shadermeta does not exist, thus the engine is not going to consider the file.
		if (!std::filesystem::exists(std::filesystem::path(metaPath.C_Str())))
			return;

		std::ifstream ifs(metaPath.C_Str());
		LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Writes resource metadata to its serialized representation.
	void FTShader::SaveMetaData()
	{
		Common::FTDS::String metaPath;
		metaPath.Assign(*GetRelativePath());
		ReplaceSuffix(metaPath, Common::FileTypes::SHADER, Common::FileTypes::SHADER_META);

		if (!std::filesystem::exists(std::filesystem::path(metaPath.C_Str())))
			Common::Debug::LogError(__LINE__, __FILE__, ".shadermeta does not exist. Creating one...");

		std::ofstream ofs(metaPath.C_Str());

		if (ofs)
		{
			SaveProperties(ofs);
			FileIOHelper::SaveBufferToFile(ofs);
		}
		else
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to save shader meta file");
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
