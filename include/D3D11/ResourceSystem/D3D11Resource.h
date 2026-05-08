#pragma once
#include "ResourceSystem/FTResource.h"

#include <iosfwd>

namespace Core
{
	struct FTResourceDef;
}

namespace D3D11
{
	enum ResType
	{
		UNSUPPORTED,
		SPRITE_ANIMATION,
		SPINE_ANIMATION,
		SHADER_META,
		FONT,
		VERTEX_SHADER,
		GEOMETRY_SHADER,
		PIXEL_SHADER,
		D3D11_PSO,
		MATERIAL,
		MESH_GROUP,
		SPRITE,
		TILEMAP,
		TEXTURE,
		END
	};

	class D3D11Resource : public Common::FTResource
	{
	public:
		inline static ResType Type;
		/// @brief Saves resource properties into a file.
		/// @param ofs This should either be a stream to a .chunk file, or to a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Loads resource properties into an instance.
		/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		D3D11Resource(Common::FTResourceDef& resDef);
		virtual ~D3D11Resource() = default;
	};
} // namespace D3D11