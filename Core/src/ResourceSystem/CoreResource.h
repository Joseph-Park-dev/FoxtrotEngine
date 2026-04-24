#pragma once
#include "ResourceSystem/FTResource.h"

#include <iosfwd>

namespace Core
{
	enum ResType
	{
		UNSUPPORTED,
		PREMADE,
		CSV,
		JSON,
		TEXT,
		END
	};

	class CoreResource : public FTResource
	{
	public:
		/// @brief Saves resource properties into a file.
		/// @param ofs This should either be a stream to a .chunk file, or to a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @brief Loads resource properties into an instance.
		/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

	public:
		CoreResource(FTResourceDef& resDef);
		virtual ~CoreResource() = default;
	};
} // namespace Core